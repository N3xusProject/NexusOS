/*
 *  Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
 *  See LICENSE file.
*/

#include <power/acpi/acpi.h>
#include <power/acpi/tables.h>
#include <libkern/log.h>
#include <libkern/panic.h>
#include <libkern/string.h>
#include <arch/pic/apic.h>
#include <limine.h>

// APIC structure types.
#define APIC_TYPE_LOCAL_APIC            0
#define APIC_TYPE_IO_APIC               1
#define APIC_TYPE_INTERRUPT_OVERRIDE    2

static acpi_rsdp_t* rsdp = NULL; 
static acpi_rsdt_t* rsdt = NULL;
static acpi_madt_t* madt = NULL;
static uint32_t rsdt_entries = 0;
static uint8_t cpu_count = 0;

void* ioapic_base = NULL;
void* bsp_lapic_base = NULL;



static volatile struct limine_rsdp_request rsdp_req = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};


/*
 *  @param header     header for an ACPI structure (ex. rsdp).
 *  @return           1 if checksum is verified, otherwise 0.
 *
 *
 */
static uint8_t verify_checksum(acpi_header_t* header)
{
    uint8_t sum = 0;

    for (int i = 0; i < header->length; ++i)
    {
        sum += ((char*)header)[i];
    }

    return sum % 0x100 == 0;
}

/*
 * @return      1 if MADT was found, otherwise 0.
 *
 */
static uint8_t find_madt(void) 
{
    for (size_t i = 0; i < rsdt_entries; ++i)
    {
        acpi_madt_t* cur = (acpi_madt_t*)(uint64_t)rsdt->tables[i];

        if (memcmp(cur->header.signature, "APIC", 4))
        {
            madt = cur;
            return 1;
        }
    }

    // MADT NOT FOUND!
    return 0;
}


/*
 *  @brief      Parses MADT structure and outputs processor and APIC information.
 *              This also counts how many CPUs there are.
 *
 */
static void parse_madt(void)
{
    uint8_t* cur = (uint8_t*)(madt + 1);
    uint8_t* end = (uint8_t*)madt + madt->header.length;

    while (cur < end)
    {
        struct APICHeader* header = (struct APICHeader*)cur;

        switch (header->type) {
            case APIC_TYPE_LOCAL_APIC:
                {
                    struct LocalAPIC* lapic = (struct LocalAPIC*)cur;
                    printf(BOLD_YELLOW "CPU found!\n");
                    printf(WHITE "--------------------------\n");
                    printf(CYAN "Processor ID: %x\n", lapic->processor_id);
                    printf(CYAN "APIC ID: %x\n", lapic->apic_id);
                    printf(WHITE "--------------------------\n\n");
                }

                ++cpu_count;
                break;
            case APIC_TYPE_IO_APIC:
                {
                    struct IOAPIC* ioapic = (struct IOAPIC*)cur;
                    printf(BOLD_YELLOW "I/O APIC found!\n");
                    printf(WHITE "--------------------------\n");
                    printf(CYAN "I/O APIC ID: %x\n", ioapic->ioapic_id);
                    printf(CYAN "Memory Mapped I/O Base: %x\n", ioapic->mmio_base);
                    printf(CYAN "Global System Interrupt Base: %x\n", ioapic->global_sysintr_base);
                    printf(WHITE "--------------------------\n");

                    ioapic_base = (void*)(uint64_t)ioapic->mmio_base;
                }

                break;
        }

        cur += header->length;
    }
}


uint32_t acpi_remap_irq(uint32_t irq)
{
    uint8_t* p = (uint8_t*)(madt + 1);
    uint8_t* end = (uint8_t*)madt + madt->header.length;

    while (p < end)
    {
        struct APICHeader* header = (struct APICHeader*)p;

        switch (header->type)
        {
            case APIC_TYPE_INTERRUPT_OVERRIDE:
                {
                    struct APICInterruptOverride* interrupt_override = (struct APICInterruptOverride*)p;

                    if (interrupt_override->source == irq)
                    {
                        // Return the GSI.
                        return interrupt_override->interrupt;
                    }
                }
        }

        p += header->length;
    }

    return irq;
}


void configure_acpi(void)
{
    // Fetch the RSDP.
    rsdp = rsdp_req.response->address;
    printf(KINFO "<ACPI_CONFIG>: ACPI RSDP fetched.\n");
    
    // Fetch RSDT.
    rsdt = (acpi_rsdt_t*)(uint64_t)rsdp->rsdtaddr;
    printf(KINFO "<ACPI_CONFIG>: ACPI RSDT fetched, verifying checksum..\n");
   
    // Ensure the RSDT checksum is valid.
    // If the RSDT checksum is invalid 
    // we will need to panic.
    if (!(verify_checksum(&rsdt->header)))
    {
        printf(KERN_PANIC "ACPI RSDT checksum invalid!\n");
        panic();
    }

    printf(KINFO "<ACPI_CONFIG>: ACPI RSDT checksum is valid!\n");

    // Get the amount of RSDT entries.
    rsdt_entries = (rsdt->header.length - sizeof(rsdt->header)) / 4;
    printf(KINFO "<ACPI_CONFIG>: Calculated ACPI RSDT entries.\n");

    // Verify MADT exists.
    if (!(find_madt()))
    {
        printf(KERN_PANIC "Could not locate ACPI MADT!");
        panic();
    }

    // Set BSP LAPIC Base.
    bsp_lapic_base = (void*)(uint64_t)madt->lapic_addr;

    printf(KINFO "<ACPI_CONFIG>: ACPI MADT located.\n\n");
    parse_madt();
    cpu_count > 1 ? printf(KINFO "%d cores found!\n", cpu_count) : printf(KINFO "1 core found!\n");
}
