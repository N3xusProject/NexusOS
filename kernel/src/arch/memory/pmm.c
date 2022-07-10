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

#include <arch/memory/pmm.h>
#include <arch/memory/mem.h>
#include <limine.h>
#include <libkern/log.h>
#include <libkern/panic.h>
#include <libkern/string.h>
#include <libkern/bitmap.h>
#include <stddef.h>
#include <stdint.h>

volatile struct limine_memmap_request mmap_req = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};


static void* placement = NULL;

void* pmm_alloc(void) 
{
    void* ret = placement;
    placement += PAGE_SIZE;
    return ret;
}


void* pmm_allocz(void)
{
    void* alloc = pmm_alloc();
    memzero(PHYS_TO_HIGHER_HALF_DATA(alloc), PAGE_SIZE);
    return alloc;
}

void pmm_init(void) 
{

    // First biggest segment
    struct limine_memmap_entry* biggest_segment = NULL;

    // Find largest segment to hold our bitmap.
    for (uint64_t i = 0; i < mmap_req.response->entry_count; ++i)
    {
        struct limine_memmap_entry* current = mmap_req.response->entries[i];

        // If we haven't set our biggest segment and current->type is usable memory,
        // set biggest segment and continue.
        if (biggest_segment == NULL && current->type == LIMINE_MEMMAP_USABLE) 
        {
            biggest_segment = current;
            continue;
        }

        if (current->type == LIMINE_MEMMAP_USABLE && current->length > biggest_segment->length)
        {
            biggest_segment = current;
        }
    }

    if (biggest_segment == NULL)
    {
        kprintf(KERN_PANIC "Not enough memory!\n");
        panic();
    }

    placement = (void*)biggest_segment->base;
}
