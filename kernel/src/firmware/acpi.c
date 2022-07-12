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

#include <firmware/acpi.h>
#include <firmware/acpidef.h>
#include <arch/cpu/cpuinfo.h>
#include <arch/apic/apic.h>
#include <libkern/log.h>
#include <libkern/panic.h>
#include <libkern/string.h>
#include <limine.h>

static volatile struct limine_rsdp_request rsdp_req = {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0
};

static acpi_rsdp_t* rsdp = NULL;
static acpi_rsdt_t* rsdt = NULL;
static acpi_madt_t* madt = NULL;
static uint32_t rsdt_entries = 0;
static uint8_t cpu_count = 0;

// MMIO base.
void* bsp_lapic_base = NULL;
void* ioapic_base;

static uint8_t locate_madt(void)
{
    // Go through all RSDT entries until we find the MADT.
    for (uint32_t i = 0; i < rsdt_entries; ++i)
    {
        acpi_madt_t* cur = (acpi_madt_t*)(uint64_t)rsdt->tables[i];

        if (strncmp(cur->header.signature, "APIC", 4) == 0)
        {
            madt = cur;
            return 1;
        }
    }

    return 0;
}


static void parse_madt(void)
{
    uint8_t* cur = (uint8_t*)(madt + 1);
    uint8_t* end = (uint8_t*)madt + madt->header.length;

    while (cur < end)
    {
        struct APICHeader* header = (struct APICHeader*)cur;

        switch (header->type)
        {
            case APIC_TYPE_LOCAL_APIC:
                {
                    struct LocalAPIC* lapic = (struct LocalAPIC*)cur;
                    kprintf(BOLD_YELLOW "-- Processor Found --\n");
                    kprintf(CYAN "Processor Local APIC ID: %x\n", lapic->apic_id);
                    kprintf(CYAN "Processor ID: %x\n", lapic->processor_id);
                    kprintf(CYAN "Processor Vendor: %s\n", cpuinfo_get_vendor());
                    kprintf(CYAN "Local APIC Flags: %x\n\n", lapic->flags);
                }

                ++cpu_count;
                break;
            case APIC_TYPE_IO_APIC:
                {
                    struct IOAPIC* ioapic = (struct IOAPIC*)cur;
                    kprintf(BOLD_YELLOW "-- I/O APIC Found --\n");
                    kprintf(CYAN "I/O APIC ID: %x\n", ioapic->ioapic_id);
                    kprintf(CYAN "I/O APIC MMIO Base: %x\n", ioapic->mmio_base);
                    kprintf(CYAN "Global System Interrupt Base: %x\n\n", ioapic->global_sysintr_base);
                    break;
                }
        }

        cur += header->length;
    }
}


static uint8_t verify_checksum(acpi_header_t* header)
{
    uint8_t sum = 0;

    for (int i = 0; i < header->length; ++i)
    {
        sum += ((char*)header)[i];
    }

    return sum % 0x100 == 0;
}

void acpi_init(void)
{
    kprintf(KINFO "\n\n<ACPI_INIT>: Fetching CPU Information..\n");
    kprintf(KINFO "CPU Vendor: %s\n", cpuinfo_get_vendor());
    kprintf(KINFO "CPU Model Number: %x\n", cpuinfo_get_model_number());
    kprintf(KINFO "CPU Family Value: %x\n\n", cpuinfo_get_family_value());

    rsdp = (acpi_rsdp_t*)(uint64_t)rsdp_req.response->address;
    rsdt = (acpi_rsdt_t*)(uint64_t)rsdp->rsdtaddr;
    rsdt_entries = (rsdt->header.length - sizeof(rsdt->header)) / 4;

    kprintf(KINFO "<ACPI_INIT>: RSDT fetched, verifying checksum..\n");

    if (!(verify_checksum(&rsdt->header)))
    {
        kprintf(KERN_PANIC "RSDT checksum invalid!\n");
        panic();
    }

    kprintf("<ACPI_INIT>: RSDT checksum is valid, continuing..\n");

    if (!(locate_madt()))
    {
        kprintf(KERN_PANIC "Could not locate MADT!\n");
        panic();
    }

    kprintf("<ACPI_INIT>: Located MADT.\n\n");
    bsp_lapic_base = (void*)(uint64_t)madt->lapic_addr;

    // Parse MADT.
    parse_madt();
}
