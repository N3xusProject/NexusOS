/*
 *  Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
 *  See LICENSE file.
*/

#include <stdint.h>
#include <stddef.h>
#include <limine.h>
#include <arch/interrupts/IDT.h>
#include <arch/memory/pmm.h>
#include <arch/memory/vmm.h>
#include <arch/pic/lapic.h>
#include <arch/pic/ioapic.h>
#include <libkern/asm.h>
#include <libkern/log.h>
#include <libkern/panic.h>
#include <libkern/driverctl.h>
#include <power/acpi/acpi.h>
#include <drivers/timers/PIT.h>
#include <drivers/sound/pcspkr.h>
#include <drivers/ata/ahci.h>


static void done(void)
{
  	for (;;) {
        __asm__("hlt");
    }
}


/*
 *  @brief  An HDD is needed for boot,
 *          this function attempts to locate 
 *          a driver for the hard drive.
 *
 *          If this function fails, then you will
 *          be met with a kernel panic.
 *
 */
static void locate_hdd_driver(void)
{
    
    if (ahci_init())
    {
        printf(KINFO "Using AHCI.\n");

        // We are using the AHCI driver for HDD.
        driverctl_set_used_driver(HDD_DRIVERCLASS, AHCI_DRIVER);
        return;
    }

    printf(KERN_PANIC "Could not locate proper HDD driver.\n");
    panic();
}


// The following will be our kernel's entry point.
void _start(void)
{
    CLI;
	// Setup Interrupt Descriptor Table.
    idt_install();
    printf(KINFO "IDT installed!\n");

	// Set up Physical Memory Manager.
    pmm_init();
    printf(KINFO "PMM initialized!\n");

    // Set Virtual Memory Manager.
    vmm_init();
    printf(KINFO "VMM initialized!\n");

    configure_acpi();
    printf(KINFO "ACPI configured!\n");

    ioapic_init();
    printf("I/O APIC initialized!\n");

    lapic_init();
    printf("LAPIC initialized!\n");

    // Setup the PIT timer.
    pit_set_phase(DEFAULT_TIMER_PHASE);
    printf("Timer phase set to %d hz\n", DEFAULT_TIMER_PHASE);

    // Setup POST IDT stuff.
    post_idt_setup();

    printf("Sleeping for 560 ticks..\n");
    CLI;                                        // CLI is temporary, pit_sleep() calls STI.
    pit_sleep(560);
    printf("Done sleeping!\n");

    CLI;
    pcspkr_play(500);
    pit_sleep(300);                             // CLI is temporary, pit_sleep() calls STI.
    pcspkr_stop(); 

    locate_hdd_driver();

    while (1);
}
