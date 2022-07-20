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

#include <arch/memory/gdt.h>
#include <libkern/log.h>
#include <proc/tss.h>
#include <stddef.h>

#define GDT_TSS 0x9

static struct GDTDesc gdt[] = {
    {0}, 

    {
        .limit       = 0xFFFF,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b10011010,
        .granularity = 0b00000000,
        .base_hi     = 0x00
    },

    {
        .limit       = 0xFFFF,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b10010010,
        .granularity = 0b00000000,
        .base_hi     = 0x00
    },

    {
        .limit       = 0xFFFF,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b10011010,
        .granularity = 0b11001111,
        .base_hi     = 0x00
    },

    {
        .limit       = 0xFFFF,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b10010010,
        .granularity = 0b11001111,
        .base_hi     = 0x00
    },

    {
        .limit       = 0x0000,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b10011010,
        .granularity = 0b00100000,
        .base_hi     = 0x00
    }, 

    {
        .limit       = 0x0000,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b10010010,
        .granularity = 0b00000000,
        .base_hi     = 0x00
    },

    // User code: 0x7
    {
        .limit       = 0x0000,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b11111010,
        .granularity = 0b10101111,
        .base_hi     = 0x00
    },

    // User data: 0x8
    {
        .limit       = 0x0000,
        .base_low    = 0x0000,
        .base_mid    = 0x00,
        .access      = 0b11110010,
        .granularity = 0b00000000,
        .base_hi     = 0x00
    },

    // 0x9.
    {0}, {0}, {0}
};


struct TSSDescriptor* gdt_tss = (struct TSSDescriptor*)&gdt[GDT_TSS];


static struct GDTR gdtr = {
    sizeof(gdt) - 1,
    (uintptr_t)gdt
};

void load_gdt(void)
{
    __asm__ __volatile__("lgdt %0" :: "m" (gdtr));
}
