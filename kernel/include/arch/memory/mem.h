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

#ifndef MEM_H
#define MEM_H

#define HIGHER_HALF_DATA	0xFFFF800000000000UL
#define HIGHER_HALF_CODE	0xFFFFFFFF80000000UL
#define GiB 0x40000000UL

#define ALIGN_DOWN(address, align)  ((address) & ~((align)-1))
#define ALIGN_UP(address, align)    (((address) + (align)-1) & ~((align)-1))

#define PAGE_SIZE 0x1000

#define BYTES_TO_PAGES(bytes) (bytes / PAGE_SIZE)

// Fetches higher-half virtual address with a physical address.
#define PHYS_TO_HIGHER_HALF_DATA(address)   (HIGHER_HALF_DATA + address)
#define PHYS_TO_HIGHER_HALF_CODE(address)   (HIGHER_HALF_CODE + address)

#define HIGHER_HALF_DATA_TO_PHYS(address)   (address - HIGHER_HALF_DATA)
#define HIGHER_HALF_CODE_TO_PHYS(address)   (address - HIGHER_HALF_CODE)

#define BIT_TO_PAGE(bit) (bit * PAGE_SIZE)
#define PAGE_TO_BIT(page_base) (page_base / PAGE_SIZE)

#endif
