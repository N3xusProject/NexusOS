/*
 *  Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
 *  See LICENSE file.
*/


// Memory mapped register for I/O APIC access.
#define IOREGSEL                        0x00
#define IOWIN                           0x10

// Other I/O APIC registers.
#define IOAPICID                        0x00
#define IOAPICVER                       0x01
#define IOAPICARB                       0x02
#define IOREDTBL                        0x10

#include <arch/pic/ioapic.h>
#include <libkern/log.h>

extern void* ioapic_base;


/*
 *  @brief  Reads an I/O APIC register by sending
 *          register number to IOREGSEL and reading
 *          IOWIN.
 *
 *  @param reg  Register to read.
 *
 *
 */

static uint32_t read(uint8_t reg)
{
    // Tell IOREGSEL which register should be read.
    *(volatile uint32_t*)(ioapic_base) = reg;

    // Read value from IOWIN.
    return *(volatile uint32_t*)(ioapic_base + IOWIN);
}


/*
 *  @brief  Writes an I/O APIC by doing the same
 *          thing as read() but instead of reading IOWIN
 *          we are writing a value to it.
 *  
 *  @param reg  The register we want to read from.
 *  @param val  The value we want to write.
 */

static void write(uint8_t reg, uint32_t val)
{
    // Tell IOREGSEL which register we want to write to.
    *(volatile uint32_t*)ioapic_base = reg;

    // Write to IOWIN which writes to the register.
    *(volatile uint32_t*)(ioapic_base + IOWIN) = val;
}


void ioapic_set_entry(uint8_t index, uint64_t data) 
{
    write(IOREDTBL + index * 2, (uint32_t)data);
    write(IOREDTBL + index * 2 + 1, (uint32_t)(data >> 32));
}


void ioapic_init(void) 
{
    // 23:16: Maximum Redirection Entries minus one.
    uint32_t redirection_entries = ((read(IOAPICVER) >> 16) & 0xFF) + 1;

    printf("<IOAPIC_INIT>: I/O APIC Interrupt Pins: %d\n", redirection_entries);

    // Mask all I/O APIC entries.
    for (uint32_t i = 0; i < redirection_entries; ++i) {
        ioapic_set_entry(i, 1 << 16);
    }

    printf("<IOAPIC_INIT>: All entries masked!\n");
}
