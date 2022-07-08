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

static volatile struct limine_memmap_request mmap_req = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};


static struct Bitmap pmm_bitmap;
static void* biggest_segment_physical = NULL;
static uint64_t pages_left = 0;


void* pmm_alloc(void) 
{
    // LIMIT (i.e max physical address of our usable segment).
    const uint64_t LIMIT = (uint64_t)biggest_segment_physical + (PAGE_SIZE * pages_left);
    for (uint32_t i = PAGE_TO_BIT((uint64_t)biggest_segment_physical); i < PAGE_TO_BIT(LIMIT); ++i)
    {
        if (!(bitmap_check_bit(&pmm_bitmap, i)))
        {
            // Memory is allocated, so set the bit.
            bitmap_set_bit(&pmm_bitmap, i);
            return (void*)(uint64_t)BIT_TO_PAGE(i);
        }
    }
}


uint8_t pmm_free(void* ptr)
{
    if (PAGE_TO_BIT((uint64_t)ptr) > (pmm_bitmap.size * 8))
    {
        return 0;
    }

    bitmap_unset_bit(&pmm_bitmap, PAGE_TO_BIT((uint64_t)ptr));
    return 1;
}

void pmm_init(void) 
{

    // Largest segment.
    struct limine_memmap_entry* second_biggest_segment = NULL;

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
            second_biggest_segment = biggest_segment;
            biggest_segment = current;
        }
    }

    if (biggest_segment == NULL || second_biggest_segment == NULL)
    {
        kprintf(KERN_PANIC "Not enough memory!\n");
        panic();
    }

    // Setup bitmap.
    pmm_bitmap.size = ALIGN_UP(ALIGN_DOWN(second_biggest_segment->base, PAGE_SIZE) / PAGE_SIZE / 8, PAGE_SIZE);
    pmm_bitmap.map = (uint8_t*)PHYS_TO_HIGHER_HALF_DATA(second_biggest_segment->base);
    kprintf("<PMM>: Bitmap size: %d bytes\n", pmm_bitmap.size);

    // Setup other global variables.
    pages_left = biggest_segment->length / PAGE_SIZE;
    biggest_segment_physical = (void*)biggest_segment->base;
}
