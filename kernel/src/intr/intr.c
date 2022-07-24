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

#include <intr/intr.h>
#include <intr/exceptions.h>
#include <intr/IDT.h>
#include <intr/irq.h>
#include <intr/syscall.h>
#include <arch/apic/ioapic.h>
#include <drivers/ps2/keyb_controller.h>
#include <firmware/acpi.h>

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


void intr_setup_irqs(void)
{
    // Timer IRQ.
    set_idt_desc(0x20, irq0, INT_GATE_FLAGS);
    ioapic_set_entry(acpi_map_irq(0), 0x20);

    // Keyboard IRQ.
    set_idt_desc(0x21, irq1_handler, INT_GATE_FLAGS);
    ioapic_set_entry(acpi_map_irq(1), 0x21);
}


void intr_init(void) 
{
     for (uint8_t i = 0; i <= 0xE; ++i) 
     {
        set_idt_desc(i, exceptions[i], TRAP_GATE_FLAGS);
     }
    
     set_idt_desc(0x80, syscall_dispatcher, IDT_INT_GATE_USER);

     idt_install();
}
