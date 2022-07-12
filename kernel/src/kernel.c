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

#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <libkern/asm.h>
#include <libkern/log.h>
#include <libkern/driverctl.h>
#include <libkern/driverinit.h>
#include <intr/intr.h>
#include <arch/memory/pmm.h>
#include <arch/memory/vmm.h>
#include <arch/apic/ioapic.h>
#include <arch/apic/lapic.h>
#include <firmware/acpi.h>
#include <drivers/clocks/PIT.h>

static void done(void)
{
  	for (;;) 
    {
        __asm__("hlt");
    }
}


static void init_drivers(void)
{
    init_hdd();
    pit_set_phase(DEFAULT_TIMER_PHASE);
    kprintf("PIT phase set at %d Hz\n", DEFAULT_TIMER_PHASE);
    __asm__ __volatile__("sti");
}


static void init(void) {
    __asm__ __volatile__("cli");
    intr_init();
    kprintf(KINFO "Interrupts initialized.\n");

    pmm_init();
    kprintf(KINFO "Physical Memory Manager initialized.\n");

    vmm_init();
    kprintf(KINFO "Virtual Memory Manager initialized.\n");

    acpi_init();
    kprintf(KINFO "ACPI initialized.\n");

    ioapic_init();
    kprintf(KINFO "I/O APIC initialized.\n");

    lapic_init();
    kprintf(KINFO "LAPIC initialized.\n");

    intr_setup_irqs();
    kprintf(KINFO "IRQs setup.\n");

    init_drivers();
    kprintf(KINFO "Drivers initialized.\n");
}

// The following will be our kernel's entry point.
void _start(void)
{
    __asm__ __volatile__("cli");
    char ascii_art[] = {0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2E, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2E, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x2F, 0x20, 0x2E, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2D, 0x2E, 0x20, 0x5C, 0x0A, 0x20, 0x20, 0x20, 0x2F, 0x20, 0x2F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x5C, 0x20, 0x5C, 0x0A, 0x20, 0x20, 0x20, 0x7C, 0x20, 0x7C, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0A, 0x20, 0x20, 0x5F, 0x7C, 0x20, 0x7C, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x0A, 0x2E, 0x27, 0x20, 0x7C, 0x5F, 0x7C, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7C, 0x5F, 0x7C, 0x20, 0x27, 0x2E, 0x0A, 0x27, 0x2E, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x20, 0x5F, 0x5F, 0x5F, 0x5F, 0x20, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x2E, 0x27, 0x0A, 0x7C, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2E, 0x27, 0x5F, 0x5F, 0x5F, 0x5F, 0x27, 0x2E, 0x20, 0x20, 0x20, 0x20, 0x20, 0x7C, 0x0A, 0x27, 0x2E, 0x5F, 0x5F, 0x2E, 0x27, 0x2E, 0x27, 0x20, 0x20, 0x20, 0x20, 0x27, 0x2E, 0x27, 0x2E, 0x5F, 0x5F, 0x2E, 0x27, 0x0A, 0x27, 0x2E, 0x5F, 0x5F, 0x20, 0x20, 0x7C, 0x20, 0x4E, 0x65, 0x78, 0x75, 0x73, 0x20, 0x7C, 0x20, 0x20, 0x5F, 0x5F, 0x2E, 0x27, 0x0A, 0x7C, 0x20, 0x20, 0x20, 0x27, 0x2E, 0x27, 0x2E, 0x5F, 0x5F, 0x5F, 0x5F, 0x2E, 0x27, 0x2E, 0x27, 0x20, 0x20, 0x20, 0x7C, 0x0A, 0x27, 0x2E, 0x5F, 0x5F, 0x5F, 0x5F, 0x27, 0x2E, 0x5F, 0x5F, 0x5F, 0x5F, 0x2E, 0x27, 0x5F, 0x5F, 0x5F, 0x5F, 0x2E, 0x27, 0x0A, 0x27, 0x2E, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x2E, 0x27, 0xA, 0xA, 0xA, 0};
    kprintf("%s%s\n\n", BOLD_YELLOW, ascii_art);

    init();

    done();
}
