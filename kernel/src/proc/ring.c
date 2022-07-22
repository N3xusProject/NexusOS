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
#include <libkern/module.h>
#include <libkern/log.h>
#include <libkern/panic.h>
#include <stddef.h>

void* ring3_entry = NULL;

void ring3(void);

void prepare_ring3(void)
{

    extern uint64_t* pml4;

    // Get the Nexus Daemon.
    struct limine_file* nexd = get_module_by_name("/Nexus/nexd.sys");

    // Some error checking.
    if (nexd == NULL)
    {
        kprintf(KERN_PANIC "NexD(aemon) could not be loaded.\n");
        panic();
    } 

    ring3_entry = (void*)0x2000;
    void* tmp = (void*)0x2000;

    // Map some low pages for our userspace code.
    for (uint64_t i = 0; i < nexd->size; i += 0x1000)
    {
        vmm_map_page(pml4, tmp, PAGE_P_PRESENT | PAGE_US_USER | PAGE_RW_WRITABLE);
        tmp += 0x1000;
    }

    uint8_t* ring3_code_src = (uint8_t*)nexd->address;
    uint8_t* ring3_code_dst = (uint8_t*)0x2000;

    for (uint8_t i = 0; i < nexd->size; ++i)
    {
        ring3_code_dst[i] = ring3_code_src[i];
    }
}
