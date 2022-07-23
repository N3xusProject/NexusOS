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

#include <libkern/log.h>
#include <libkern/panic.h>
#include <libkern/string.h>
#include <arch/memory/vmm.h>
#include <arch/memory/mem.h>
#include <stddef.h>

#define KHEAP_START 0x500
#define KHEAP_LIMIT 0x500+GiB
#define KHEAP_END 0x500+(KHEAP_LIMIT+1)
#define KHEAP_MAGIC 0xCA75101               // CATS LOL
#define DATA_START(mem_block) (((char*) mem_block) + sizeof(struct Block))

struct __attribute__((packed)) Block 
{
    uint8_t is_free : 1;
    struct Block* next;
    size_t size;
    uint32_t magic;
};

static struct Block* mem_head = NULL;
static struct Block* mem_tail = NULL;
static size_t bytes_allocated = 0;

extern uint64_t* pml4;


void kheap_init(void) 
{
    vmm_map_page(pml4, (void*)KHEAP_START, PAGE_P_PRESENT | PAGE_RW_WRITABLE);
    mem_head = (struct Block*)KHEAP_START;
    mem_head->next = NULL;
    mem_head->size = 0;
    mem_head->is_free = 0;
    mem_head->magic = KHEAP_MAGIC;
    mem_tail = mem_head;
}


static struct Block* first_fit(size_t size) 
{
    struct Block* cur_block = mem_head;

    while (cur_block != NULL)
    {
        if (cur_block->is_free && cur_block->size >= size)
        {
            return cur_block;
        }

        cur_block = cur_block->next;
    }

    // No memory found.
    return NULL;
}

void* kmalloc(size_t sz)
{
    if (bytes_allocated + sz > KHEAP_LIMIT)
    {
        return NULL;
    }

    struct Block* region = first_fit(sz);

    // Map the whole region.
    void* tmp = region;
    for (uint64_t i = 0; i < sz; i += 0x1000)
    {
        vmm_map_page(pml4, tmp, PAGE_P_PRESENT | PAGE_RW_WRITABLE | PAGE_US_USER);
        tmp += 0x1000;
    }

    if (region == NULL)
    {
        char* next = DATA_START(mem_tail + mem_tail->size);
        mem_tail->next = (struct Block*)next;
        region = mem_tail->next;
        region->size = sz;
        region->is_free = 0;
        region->next = NULL;
        region->magic = KHEAP_MAGIC;
        mem_tail = region;
        bytes_allocated += sz;
    }

    return DATA_START(region);
}

void kfree(void* block)
{
    struct Block* cur_block = mem_head;

    while (cur_block != NULL && block != DATA_START(cur_block))
    {
        cur_block->is_free = 1;
        cur_block = cur_block->next;
    }
}


void* krealloc(void* old_block, size_t sz)
{
    struct Block* old_block_data = (struct Block*)(old_block - sizeof(struct Block));

    if (old_block_data->magic != KHEAP_MAGIC)
    {
        kprintf(PURPLE "Bad call to realloc() or heap corruption, rejecting.\n");
        return NULL;
    }

    void* new_block = kmalloc(sz);
    strncpy(new_block, old_block, old_block_data->size);
    kfree(old_block);
    return new_block;
}
