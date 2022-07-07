/*
 *  Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
 *  See LICENSE file.
 */


#include <drivers/timers/PIT.h>
#include <arch/io/io.h>
#include <libkern/asm.h>

void pit_set_phase(uint16_t hz)
{
    int divisor = 1193180 / hz;         // Calculate divisor.
    outportb(0x43, 0x36);               // Command byte.
    outportb(0x40, divisor & 0xFF);     // Low byte of divisor.
    outportb(0x40, divisor >> 8);       // High byte of divisor.
}


void pit_sleep(uint16_t ticks)
{
    extern uint16_t PIT_TICKS;
    uint16_t e_ticks = PIT_TICKS + ticks;

    STI;
    while (PIT_TICKS < e_ticks);
}
