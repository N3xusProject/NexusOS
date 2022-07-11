/*
 *  MIT License
 *
 *  Copyright (c) 2022 N3xusProject, Ian Marco Moffett
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 */

#include <drivers/ata/ahci_controller.h>
#include <drivers/ata/ahci_def.h>
#include <arch/bus/pci/device.h>
#include <arch/bus/pci/access.h>
#include <arch/memory/mem.h>
#include <libkern/log.h>
#include <stddef.h>
#include <stdint.h>

static struct PCIDevice hba;
static volatile HBA_MEM* abar = NULL;
static uint8_t port_count = 0;
static HBA_PORT* ports[32];


/*  
 * @brief   Logs device type and saves it in an array.
 *
 */
static void get_dev_type(uint8_t portno)
{
    HBA_PORT* port = &abar->ports[portno];

    uint32_t sata_status = port->ssts;
    uint8_t ipm = (sata_status >> 8) & 0x0F;
    uint8_t det = sata_status & 0x0F;

    if (det != HBA_PORT_DET_PRESENT || ipm != HBA_PORT_IPM_ACTIVE)
    {
        return;
    }

    ports[port_count++] = port;

    switch (port->sig)
    {
        case SATA_SIG_ATAPI:
            kprintf("<AHCI_DRIVER>: ATAPI drive found on port %d\n", portno);
            break;
        case SATA_SIG_SEMB:
            kprintf("<AHCI_DRIVER>: SEMB drive found on port %d\n", portno);
            break;
        case SATA_SIG_PM:
            kprintf("<AHCI_DRIVER>: Port multiplier found on port %d\n", portno);
            break;
        default:
            kprintf("<AHCI_DRIVER>: SATA drive found on port %d\n", portno);
            break;
    }
}

static void probe_ports(void)
{
    uint8_t bit = 0;
    uint32_t pi = abar->pi;

    while (bit < 32)
    {
        if (pi & 1)
        {
            get_dev_type(bit);
        }

        ++bit;
        pi >>= 1;
    }
}


uint8_t ahci_sata_exists(void)
{
    for (uint32_t i = 0; i < port_count; ++i)
    {
        if (ports[i]->sig == SATA_SIG_ATA)
        {
            return 1;
        }
    }

    return 0;
}

uint8_t ahci_hba_init(void)
{
    hba = pci_device_lookup(MASS_STORAGE_CONTROLLER, SERIAL_ATA_CONTROLLER);

    if (!(hba.valid))
    {
        return 0;
    }
    
    abar = (volatile HBA_MEM*)(uint64_t)pci_get_bar5(hba.bus, hba.slot, hba.func);
    kprintf("<AHCI_DRIVER>: AHCI Host Bus Adapter found on PCI bus %x, slot %x\n", hba.bus, hba.slot);
    kprintf("<AHCI_DRIVER>: Probing ports..\n");
    probe_ports();

    return 1;
}
