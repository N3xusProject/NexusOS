/*
 *  Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
 *  See LICENSE file.
*/


#include <arch/interrupts/IDT.h>
#include <arch/interrupts/exceptions.h>
#include <arch/interrupts/irq.h>
#include <arch/pic/ioapic.h>
#include <power/acpi/acpi.h>

#define TRAP_GATE_FLAGS 0x8F
#define INT_GATE_FLAGS 0x8E
#define IDT_INT_GATE_USER 0xEE


static struct InterruptGateDescriptor idt[256];
static struct IDTR idtr;


static void set_idt_desc(uint8_t vector, void* isr, uint32_t flags) {
    uint64_t addr = (uint64_t)isr;
    struct InterruptGateDescriptor* vec = &idt[vector];
    vec->isr_low16 = addr & 0xFFFF;
    vec->isr_mid16 = (addr >> 16) & 0xFFFF;
    vec->isr_high32 = (addr >> 32);
    vec->p = 1;
    vec->attr = flags;
    vec->cs = 0x28;
    vec->ist = 0;
    vec->dpl = 3;
    vec->reserved = 0;
    vec->zero = 0;
    vec->zero1 = 0;
    vec->zero2 = 0;
}


static void(*exceptions[])(void) = {
    divide_error,
    debug_exception,
    general_protection_fault,
    general_protection_fault,
    overflow,
    bound_range_exceeded,
    invalid_opcode,
    no_mathcoprocessor,
    double_fault,
    general_protection_fault,
    invalid_tss,
    segment_not_present,
    stack_segment_fault,
    general_protection_fault,
    page_fault
};


static void setup_exceptions(void) {
    for (uint8_t i = 0; i <= 0xE; ++i) {
        set_idt_desc(i, exceptions[i], TRAP_GATE_FLAGS);
    }
}


void post_idt_setup(void) {
    // Setup PIT interrupt vector.
    set_idt_desc(0x20, isr_irq0, INT_GATE_FLAGS);

    // Setup PIT timer IRQ.
    ioapic_set_entry(acpi_remap_irq(0), 0x20);
}


void idt_install(void) {
    setup_exceptions();
    idtr.limit = sizeof(struct InterruptGateDescriptor) * 256 - 1;
    idtr.base = (uint64_t)&idt;
    __asm__ __volatile__("lidt %0" :: "m" (idtr));
}
