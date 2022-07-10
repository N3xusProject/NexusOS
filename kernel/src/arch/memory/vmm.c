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

#include <arch/memory/vmm.h>
#include <arch/memory/pmm.h>
#include <arch/memory/mem.h>
#include <libkern/log.h>
#include <limine.h>
#include <stddef.h>

#define PAGE_ADDR_MASK 0x000FFFFFFFFFF000
#define GiB 0x40000000UL

struct MappingTable* pml4;


static void invlpg(void* addr)
{

    __asm__ __volatile__("invlpg (%0)" : : "r" (addr));
}

void vmm_map_page(struct MappingTable* _pml4, void* logical, uint32_t flags)
{
    // Ensure one of the flags is PAGE_P_PRESENT.
    flags |= PAGE_P_PRESENT;
    uint64_t addr = (uint64_t)logical;

    int pml4_idx = (addr >> 39) & 0x1FF;
    int pdpt_idx = (addr >> 30) & 0x1FF;
    int pd_idx = (addr >> 21) & 0x1FF;
    int pt_idx = (addr >> 12) & 0x1FF;

    if (!(_pml4->entries[pml4_idx] & PAGE_P_PRESENT))
    {
        _pml4->entries[pml4_idx] = ((uint64_t)pmm_allocz() & PAGE_ADDR_MASK) | flags;
    }

    struct MappingTable* pdpt = (struct MappingTable*)(_pml4->entries[pml4_idx] & PAGE_ADDR_MASK);

    if (!(pdpt->entries[pdpt_idx] & PAGE_P_PRESENT))
    {
        pdpt->entries[pdpt_idx] = ((uint64_t)pmm_allocz() & PAGE_ADDR_MASK) | flags;
    }

    struct MappingTable* pd = (struct MappingTable*)(pdpt->entries[pdpt_idx] & PAGE_ADDR_MASK);

    if (!(pd->entries[pd_idx] & PAGE_P_PRESENT))
    {
        pd->entries[pd_idx] = ((uint64_t)pmm_allocz() & PAGE_ADDR_MASK) | flags;
    }

    struct MappingTable* pt = (struct MappingTable*)(pd->entries[pd_idx] & PAGE_ADDR_MASK);

    if (!(pt->entries[pt_idx] & PAGE_ADDR_MASK))
    {
        pt->entries[pt_idx] = (addr & PAGE_ADDR_MASK) | flags;
    }

    invlpg((void*)(uint64_t)ALIGN_DOWN((uint64_t)logical, PAGE_SIZE));
    __asm__ __volatile__("mov %0, %%cr3" :: "r" (_pml4));
}


void vmm_unmap_page(struct MappingTable* pml4, void* logical)
{
    uint64_t addr = (uint64_t)ALIGN_DOWN((uint64_t)logical, PAGE_SIZE);

    int pml4_idx = (addr >> 39) & 0x1FF;
    int pdpt_idx = (addr >> 30) & 0x1FF;
    int pd_idx = (addr >> 21) & 0x1FF;
    int pt_idx = (addr >> 12) & 0x1FF;

    struct MappingTable* pdpt = (struct MappingTable*)(pml4->entries[pml4_idx] & PAGE_ADDR_MASK);
    struct MappingTable* pd = (struct MappingTable*)(pdpt->entries[pdpt_idx] & PAGE_ADDR_MASK);
    struct MappingTable* pt = (struct MappingTable*)(pd->entries[pd_idx] & PAGE_ADDR_MASK);

    pt->entries[pt_idx] = 0;
    invlpg((void*)logical);
    __asm__ __volatile__("mov %0, %%cr3" :: "r" (pml4));
}



void load_pml4(void* pml4_phys);


void vmm_init(void)
{
    uint64_t pmm_phys = (uint64_t)pmm_allocz();
    pml4 = (void*)pmm_phys;

    __asm__ __volatile__("mov %%cr3, %0" : "=r" (pml4));
    __asm__ __volatile__("mov %0, %%cr3" :: "r" (pml4));

    vmm_map_page(pml4, 0, PAGE_P_PRESENT);
    int a = *(char*)0;

    kprintf("<VMM>: Loaded CR3\n");
}