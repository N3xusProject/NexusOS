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

#include <proc/tss.h>
#include <arch/memory/mem.h>
#include <arch/memory/vmm.h>
#include <arch/memory/gdt.h>
#include <libkern/string.h>
#include <libkern/log.h>

static struct TSSEntry tss;

void write_tss(void)
{
    extern uint64_t* pml4;
    extern struct TSSDescriptor* gdt_tss;

    vmm_map_page(pml4, (void*)0x1000, PAGE_P_PRESENT | PAGE_US_USER | PAGE_RW_WRITABLE);
    uint64_t stack = 0x1000 + (PAGE_SIZE - 1);

    memzero(&tss, sizeof(struct TSSEntry));
    tss.rsp0Low = stack & 0xFFFFFFFF;
    tss.rsp0High = (stack >> 32);

    uint64_t tssp = (uint64_t)&tss;
    gdt_tss->seglimit = sizeof(struct TSSDescriptor) - 1;
    gdt_tss->baseAddrLow = tssp & 0xFFFF;
    gdt_tss->baseAddrMiddle = (tssp >> 16) & 0xFF;
    gdt_tss->baseAddrUpper = (tssp >> 24);
    gdt_tss->type = 0x9;
    gdt_tss->zero = 0;
    gdt_tss->dpl = 3;
    gdt_tss->p = 1;
    gdt_tss->g = 1;
}


void load_tss(void)
{
    __asm__ __volatile__(
            "\
            str %bx; \
            mov $0x48, %bx; \
            ltr %bx;");
}
