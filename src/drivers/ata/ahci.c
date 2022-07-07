/*
 *  Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
 *  See LICENSE file.
*/

#include <drivers/ata/ahci.h>
#include <drivers/ata/ahcidef.h>
#include <arch/bus/pci/device.h>
#include <arch/bus/pci/access.h>
#include <arch/memory/vmm.h>
#include <libkern/string.h>
#include <libkern/log.h>
#include <stddef.h>

#define SATA_SIG_ATA 0x00000101	// SATA drive
#define SATA_SIG_ATAPI 0xEB140101	// SATAPI drive
#define SATA_SIG_SEMB 0xC33C0101	// Enclosure management bridge
#define SATA_SIG_PM 0x96690101	// Port multiplier 

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3


static struct PCIDevice ahci_controller;
static volatile HBA_MEM* abar = NULL;

static struct Drive 
{
    __attribute__((aligned(0x400))) char clb[1024];
    __attribute__((aligned(0x100))) char fb[256];
    __attribute__((aligned(0x80)))  char ctba[8000];
    AHCI_DEV_TYPE type;
    HBA_PORT* port;
} drives[32];


static uint8_t drive_count = 0;
static int allocate_cmd_slot(HBA_PORT* port)
{
    // If the bit in SACT and CI is not set, the slot is free.
    uint32_t slots = port->sact | port->ci;

    for (unsigned int i = 0; i < 32; ++i)
    {
        if (!(slots & 1))
        {
            return i;
        }

        slots >>= 1;
    }

    return -1;
}


static void initalize_drive(uint8_t drive_index)
{
    // Clearing ST (Start, bit 0) and FRE (FIS recieve enable, bit 8) bit to stop 
    // command engine.
    drives[drive_index].port->cmd &= ~(1 << 0 | 1 << 8);

    // Wait for FR (FIS Recieve Running, bit 14) and
    // CR (Command List Running, bit 15) to clear.
    while ((drives[drive_index].port->cmd & 1 << 14) && (drives[drive_index].port->cmd & 1 << 15));
    
    // Get physical address of CLB memory allocated
    // in drive struct and set lower dword in 
    // port->clb and upper dword port->clbu.
    uint64_t clb_phys = (uint64_t)vmm_get_physical(&drives[drive_index].clb);
    drives[drive_index].port->clb = (clb_phys & 0xFFFFFFFF);
    drives[drive_index].port->clbu = (clb_phys >> 32);

    // Get physical address of FB memory allocated
    // in drive struct and set lower dword in
    // port->fb and upper dword port->fbu.
    uint64_t fb_phys = (uint64_t)vmm_get_physical(&drives[drive_index].fb);
    drives[drive_index].port->fb = (fb_phys & 0xFFFFFFFF);
    drives[drive_index].port->fbu = (fb_phys >> 32);

    // Command table size = 256*32 = 8K per port.
    struct HBA_CMD_HEADER* cmdheader = (struct HBA_CMD_HEADER*)&drives[drive_index].clb;

    // Zero ctba (Command Table[Base Address]).
    memzero(drives[drive_index].ctba, 8000);

    // Get physical address of CTBA 
    uint64_t ctba_phys = (uint64_t)vmm_get_physical(&drives[drive_index].ctba);

    // Loop through all command headers.
    for (int i = 0; i < 32; ++i)
    {
        cmdheader[i].ctba = (ctba_phys & 0xFFFFFFFF);
        cmdheader[i].ctbau = (ctba_phys >> 32);
        cmdheader[i].prdtl = 8;
    }

    // Set start bit and FRE bit to start command engine.
    drives[drive_index].port->cmd |= (1 << 0 | 1 << 8);
}


static unsigned int check_dev_type(HBA_PORT* port)
{
    uint32_t sata_status = port->ssts;
    uint8_t ipm = (sata_status >> 8) & 0x0F;
    uint8_t det = sata_status & 0x0F;

    if (det != HBA_PORT_DET_PRESENT || ipm != HBA_PORT_IPM_ACTIVE)
    {
        return AHCI_DEV_NULL;
    }

    switch (port->sig)
    {
        case SATA_SIG_ATAPI:
            return AHCI_DEV_SATAPI;
        case SATA_SIG_SEMB:
            return AHCI_DEV_SEMB;
        case SATA_SIG_PM:
            return AHCI_DEV_PM;
        default:
            return AHCI_DEV_SATA;
    }
}


static void probe_ports(void)
{
    // Search for disks in implemented ports.
    uint32_t pi = abar->pi;
    uint32_t i = 0;
    drive_count = 0;

    while (i < 32)
    {
        if (pi & 1)
        {
            int dt = check_dev_type(&abar->ports[i]);

            switch (dt)
            {
                case AHCI_DEV_SATA:
                    printf(KINFO "<AHCI_DRIVER>: SATA drive found at port %d\n", i);
                    drives[drive_count].type = AHCI_DEV_SATA;
                    drives[drive_count++].port = &abar->ports[i];
                    break;
                case AHCI_DEV_SATAPI:
                    printf(KINFO "<AHCI_DRIVER>: SATAPI drive found at port %d\n", i);
                    drives[drive_count].type = AHCI_DEV_SATAPI;
                    drives[drive_count++].port = &abar->ports[i];
                    break;
                case AHCI_DEV_SEMB:
                    printf(KINFO "<AHCI_DRIVER>: SEMB drive found at port %d\n", i);
                    drives[drive_count].type = AHCI_DEV_SEMB;
                    drives[drive_count++].port = &abar->ports[i];
                    break;
                case AHCI_DEV_PM:
                    printf(KINFO "<AHCI_DRIVER>: PM drive found at port %d\n", i);
                    drives[drive_count].type = AHCI_DEV_PM;
                    drives[drive_count++].port = &abar->ports[i];
                    break;
            }
        }

        pi >>= 1;
        ++i;
    }
}


static void read(uint64_t start, uint32_t count, uint16_t* buf)
{
    HBA_PORT* port = drives[0].port;
    int cmd_slot = allocate_cmd_slot(port);
    struct HBA_CMD_HEADER* cmdheader = (struct HBA_CMD_HEADER*)&drives[0].clb;

    cmdheader[cmd_slot].cfl = sizeof(struct FIS_REG_H2D) / sizeof(uint32_t);

    // Write to 0 since we are not writing.
    cmdheader[cmd_slot].w = 0;

    struct HBA_CMD_TABLE* cmdtable = (struct HBA_CMD_TABLE*)(uint64_t)cmdheader[cmd_slot].ctba;
    uint64_t buf_phys = (uint64_t)buf;
    cmdtable->prdt_entry[0].dba = (uint32_t)buf_phys;
    cmdtable->prdt_entry[0].dbau = buf_phys >> 32;
    cmdtable->prdt_entry[0].dbc = 512 * (count - 1);
    cmdtable->prdt_entry[0].i = 1;

    struct FIS_REG_H2D* cmdfis = (struct FIS_REG_H2D*)(uint64_t)cmdtable->cfis;
    cmdfis->fis_type = 0x27;         // Host to device.
    cmdfis->c = 1;
    cmdfis->command = 0x25;          // Read.

    cmdfis->lba0 = (uint8_t)start;
    cmdfis->lba1 = (uint8_t)(start >> 8);
    cmdfis->lba2 = (uint8_t)(start >> 16);
    cmdfis->device = 1 << 6;                    // LBA mode.
    cmdfis->lba3 = (uint8_t)(start >> 24);
    cmdfis->lba4 = (uint8_t)(start >> 32);
    cmdfis->lba5 = (uint8_t)(start >> 40);
    cmdfis->countl = count & 0xFF;
    cmdfis->counth = (count >> 8) & 0xFF;

    while ((port->tfd & (1 << 7)) && (port->tfd & (1 << 3)));
    port->ci = 1 << cmd_slot;

    while ((port->ci & (1 << cmd_slot)) != 0)
    {
        if (port->is & (1 << 30))
        {
            printf(BOLD_YELLOW "<AHCI_DRIVER>: Disk read failure.\n");
            break;
        }
    }
}



uint8_t ahci_init(void) 
{
    ahci_controller = pci_locate_device(MASS_STORAGE_CONTROLLER, SERIAL_ATA_CONTROLLER);

    if (!(ahci_controller.valid))
    {
        return 0;
    }

    abar = (volatile HBA_MEM*)(uint64_t)pci_get_bar5(ahci_controller.bus, ahci_controller.slot, ahci_controller.func);
    probe_ports();
    printf(KINFO "<AHCI_DRIVER>: Drives found: %d\n", drive_count);
    printf(KINFO "<AHCI_DRIVER>: Starting init of present drives..\n");

    for (uint8_t i = 0; i < drive_count; ++i) {
        initalize_drive(i);
    }

    printf(KINFO "<AHCI_DRIVER>: All drives initialized!\n");

    __attribute__((aligned(0x1000))) uint16_t buf[512];
    vmm_map_page(buf, PAGE_P_PRESENT | PAGE_RW_WRITABLE);
    read(1, 1, buf);

    return 1;
}
