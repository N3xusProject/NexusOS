/*
 *  Copyright (c) 2022 Nexus-C, Ian Marco Moffett, All Rights Reserved.
 *  See LICENSE file.
*/

#include <arch/memory/vmm.h>
#include <arch/memory/pmm.h>
#include <arch/memory/mem.h>
#include <libkern/log.h>
#include <libkern/string.h>
#include <libkern/asm.h>
#include <stdint.h>
#include <stddef.h>


struct __attribute__((packed)) MappingTable
{
    uint64_t entries[512];
};


/*
 *	Original PML4 contents.
 *
 *	When creating a new PML4, contents from here
 *	must be copied over.
 *
 */
// static struct MappingTable orig_pml4;

// First PML4 that is loaded for kernel.
static struct MappingTable* pml4;

/*
 * 	Copies contents of PML4 to orig_pml4.
 * 	Used for setting up VMM.
 *
 * 	This also sets up base_pml4.
 *
 */

static void cr3_to_orig(void)
{
    __asm__ __volatile__("mov %%cr3, %0" : "=r" (pml4));
}



void vmm_map_page(void* logical, int flags)
{
	// Convert pointer to an integer.
	uint64_t addr = (uint64_t)logical;

    // Get indices from logical address.
    int pml4_idx = (addr >> 39) & 0x1FF;
    int pdpt_idx = (addr >> 30) & 0x1FF;
    int pd_idx = (addr >> 21) & 0x1FF;
    int pt_idx = (addr >> 12) & 0x1FF;

	if (!(pml4->entries[pml4_idx] & PAGE_P_PRESENT)) {
        // We did not define a PDPT for this entry in PML4.
        uint64_t pdpt = (uint64_t)pmm_alloc_framez();
        memzero((void*)pdpt, PAGE_FRAME_SIZE);
        pml4->entries[pml4_idx] = (pdpt & PAGE_ADDR_MASK) | flags;
        vmm_map_page((void*)pdpt, flags);
    }

    struct MappingTable* pdpt = (struct MappingTable*)(pml4->entries[pml4_idx] & PAGE_ADDR_MASK);

    if (!(pdpt->entries[pdpt_idx] & PAGE_P_PRESENT)) {
        // We did not define PDT for this PDPT entry, so allocate a page for the PDT.
        uint64_t pdt = (uint64_t)pmm_alloc_framez();
        memzero((void*)pdt, PAGE_FRAME_SIZE);
        pdpt->entries[pdpt_idx] = (pdt & PAGE_ADDR_MASK) | flags;
        vmm_map_page((void*)pdt, flags);
    }


    struct MappingTable* pdt = (struct MappingTable*)(pdpt->entries[pdpt_idx] & PAGE_ADDR_MASK);

    if (!(pdt->entries[pd_idx] & PAGE_P_PRESENT)) {
        // We did not define a PT for this PDT entry, so allocate a page for the PT.
        uint64_t pt = (uint64_t)pmm_alloc_framez();
        memzero((void*)pt, PAGE_FRAME_SIZE);
        pdt->entries[pd_idx] = (pt & PAGE_ADDR_MASK) | flags;
        vmm_map_page((void*)pt, flags);
    }

    struct MappingTable* pt = (struct MappingTable*)(pdt->entries[pd_idx] & PAGE_ADDR_MASK);

    if (!(pt->entries[pt_idx] & PAGE_P_PRESENT)) {
        pt->entries[pt_idx] = (addr & PAGE_ADDR_MASK) | flags;
    }
}


uint8_t vmm_unmap_page(void* logical) {
    uint64_t addr = (uint64_t)logical;
    if (addr % 0x1000 != 0) return 0;           // Return 0 (false) if address is not page aligned.

    // Get indices from logical address.
    int pml4_idx = (addr >> 39) & 0x1FF;
    int pdpt_idx = (addr >> 30) & 0x1FF;
    int pd_idx = (addr >> 21) & 0x1FF;
    int pt_idx = (addr >> 12) & 0x1FF;

    struct MappingTable* pdpt = (struct MappingTable*)(pml4->entries[pml4_idx] & PAGE_ADDR_MASK);
    struct MappingTable* pdt = (struct MappingTable*)(pdpt->entries[pdpt_idx] & PAGE_ADDR_MASK);
    struct MappingTable* pt = (struct MappingTable*)(pdt->entries[pd_idx] & PAGE_ADDR_MASK);

    if (pt->entries[pt_idx] & PAGE_P_PRESENT) {
        pt->entries[pt_idx] = ((addr & PAGE_ADDR_MASK) & ~(PAGE_P_PRESENT));        // Just unset the PRESENT bit.
        __asm__ __volatile__("invlpg (%0)" : : "r" (logical));
    }

    return 1;
}


void* vmm_get_physical(void* logical) {
    uint64_t addr = (uint64_t)logical;

    // Get indices from logical address.
    int pml4_idx = (addr >> 39) & 0x1FF;
    int pdpt_idx = (addr >> 30) & 0x1FF;
    int pd_idx = (addr >> 21) & 0x1FF;
    int pt_idx = (addr >> 12) & 0x1FF;


    struct MappingTable* pdpt = (struct MappingTable*)(pml4->entries[pml4_idx] & PAGE_ADDR_MASK);
    struct MappingTable* pdt = (struct MappingTable*)(pdpt->entries[pdpt_idx] & PAGE_ADDR_MASK);
    struct MappingTable* pt = (struct MappingTable*)(pdt->entries[pd_idx] & PAGE_ADDR_MASK);

    uint64_t phys = pt->entries[pt_idx];
    phys >> 12;
    phys &= 0xFFFFFFFFFFFFF;
    phys -= 1;

    return (void*)phys;
}


void load_pml4(void* pml4);


void vmm_init(void) {
	cr3_to_orig();
	printf("Got original PML4 from CR3 register!\n");
    vmm_unmap_page((void*)0x1000);
    load_pml4(pml4);
}
