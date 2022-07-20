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


#include <proc/ring.h>
#include <arch/memory/vmm.h>

void ring3(void);

void prepare_ring3(void)
{
    extern uint64_t* pml4;
    // Allocate a low page for userspace code.
    vmm_map_page(pml4, (void*)0x2000, PAGE_P_PRESENT | PAGE_RW_WRITABLE | PAGE_US_USER);

    uint8_t* ring3_code_src = (uint8_t*)ring3;
    uint8_t* ring3_code_dst = (uint8_t*)0x2000;

    for (uint8_t i = 0; i < 0x1000 && ring3_code_src[i] != 0; ++i)
    {
        ring3_code_dst[i] = ring3_code_src[i];
    }
}
