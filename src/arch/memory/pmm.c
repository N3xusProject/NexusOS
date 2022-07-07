/*
 *  Copynext (c) 2022 Nexus-C, Ian Marco Moffett, All Rights Reserved.
 *  See LICENSE file.
*/

#include <arch/memory/pmm.h>
#include <arch/memory/mem.h>
#include <libkern/log.h>
#include <libkern/panic.h>
#include <libkern/string.h>
#include <limine.h>
#include <stdint.h>
#include <stddef.h>


static volatile struct limine_memmap_request mmap_req = {
	.id = LIMINE_MEMMAP_REQUEST,
	.revision = 0
};



uint64_t largest_seg_length;
uint64_t placement = 0;
uint64_t bytes_allocated = 0;

void pmm_init(void)
{
	/*
	 *	Now we must allocate some memory
	 *	for our bitmap. We will start
	 *	the largest chunk of contiguous 
	 *	memory for our bitmap.
	 */

	printf(KINFO "<PMM>: Locating biggest chunk of contiguous memory..\n");
	struct limine_memmap_entry* biggest_entry = mmap_req.response->entries[0];

	printf(YELLOW "<usable_memory>\n");
	// Iterate through mmap entries.
	for (uint64_t i = 1; i < mmap_req.response->entry_count; ++i)
	{
		struct limine_memmap_entry* cur_entry = mmap_req.response->entries[i];
		if (cur_entry->type == LIMINE_MEMMAP_USABLE)
		{
			printf(BLUE "	%x: %x bytes" KINFO "[USABLE]\n", cur_entry->base, cur_entry->length);

			if (cur_entry->length > biggest_entry->length)
			{
				biggest_entry = cur_entry;
			}
	
		}
		else
		{
			// Not usable.
			printf(BLUE "	%x: %x bytes" YELLOW "[RESERVED]\n", cur_entry->base, cur_entry->length);
		}
	}

	printf(YELLOW "</usable_memory>\n\n");

	// Do some error checking.
	if (biggest_entry->type != LIMINE_MEMMAP_USABLE)
	{
		printf(KERN_PANIC "Failed to initialize the physical memory manager!\n");
		panic();
	}

	// Located chunk!
	printf(KINFO "<PMM>: Chunk located at %x\n", biggest_entry->base);

	/*
	 *	Now our next step is to set
	 *	the bitmap to be at the biggest
	 *	memory chunk's address.
	 */

	largest_seg_length = biggest_entry->length;
    placement = biggest_entry->base;
}


void* pmm_alloc_frame(void)
{
    if (bytes_allocated > largest_seg_length)
    {
        return NULL;
    }

    void* ret = (void*)placement;
    placement += PAGE_FRAME_SIZE;
    bytes_allocated += PAGE_FRAME_SIZE;
    return ret;
}


void* pmm_alloc_framez(void)
{
	void* frame = pmm_alloc_frame();
	memzero(frame, PAGE_FRAME_SIZE);
	return frame;
}
