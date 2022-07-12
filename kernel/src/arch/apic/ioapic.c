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

#include <arch/apic/ioapic.h>
#include <libkern/log.h>

// Memory mapped register for I/O APIC access.
#define IOREGSEL                        0x00
#define IOWIN                           0x10

// Other I/O APIC registers.
#define IOAPICID                        0x00
#define IOAPICVER                       0x01
#define IOAPICARB                       0x02
#define IOREDTBL                        0x10

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
    uint8_t pin_count = ((read(IOAPICVER) >> 16) & 0xFF) + 1;
    kprintf("<IOAPIC INIT>: I/O APIC Pins: %d\n", pin_count);

    // Mask all IRQs (bit 16 is mask bit).
    for (uint8_t i = 0; i < pin_count; ++i)
    {
        ioapic_set_entry(i, 1 << 16);
    }

    kprintf("<IOAPIC INIT>: Set masked bit on all redirection entries.\n");
}
