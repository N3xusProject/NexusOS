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

#ifndef VMM_H
#define VMM_H

#include <stdint.h>

typedef enum
{
    PAGE_P_PRESENT = (1 << 0),
    PAGE_RW_WRITABLE = (1 << 1),
    PAGE_US_USER = (1 << 2)
} PAGE_BIT;



void vmm_init(void);

 
/*
 *  NOTE: It is usually a good idea to unmap a page before mapping it to ensure correctness of
 *  this function.
 */

void vmm_map_page(uint64_t* pml4, void* logical, uint32_t flags);
void vmm_unmap_page(uint64_t* pml4, void* logical);

/*
 *  To destroy a PML4, just call pmm_free on it.
 *
 */
uint64_t* vmm_mkpml4(void);

/*
 *  Allocates a page.
 *
 */
void* vmm_alloc_page(void);

#endif
