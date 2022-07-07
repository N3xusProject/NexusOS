/*
 *  Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
 *  See LICENSE file.
*/

#include <drivers/sound/pcspkr.h>
#include <drivers/timers/PIT.h>
#include <arch/io/io.h>

void pcspkr_play(uint16_t hz)
{
    pit_set_phase(hz);

    // Play audio through pcspkr.
    uint8_t tmp = inportb(0x61);

    if (!(tmp & 3))
    {
        outportb(0x61, tmp | 3);
    }
}

// Stops pcspkr.
void pcspkr_stop(void)
{
    uint8_t tmp = inportb(0x61) & 0xFC;
    outportb(0x61, tmp);
}
