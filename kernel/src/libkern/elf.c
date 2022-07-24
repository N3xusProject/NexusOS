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

#include <libkern/elf.h>
#include <libkern/module.h>
#include <libkern/string.h>
#include <arch/memory/kheap.h>
#include <arch/memory/vmm.h>
#include <proc/thread.h>
#include <proc/ring.h>

static Elf64_Phdr* progHeaders;
extern struct Thread* current_thread;
extern uint64_t* pml4;

void* elf_get_entry(const char* path)
{
    struct limine_file* elf = get_module_by_name(path);

    if (elf == NULL)
    {
        return NULL;
    }

    char* elf_ptr = (char*)elf->address;
    char* orig_elf_ptr = elf_ptr;

    Elf64_Ehdr header;
    char* header_ptr = (char*)&header;

    // Copy Ehdr over.
    for (uint64_t i = 0; i < sizeof(header); ++i)
    {
        header_ptr[i] = elf_ptr[i];
    }

    if (strncmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
            header.e_ident[EI_CLASS] != ELFCLASS64 ||
            header.e_type != ET_EXEC ||
            header.e_machine != EM_X86_64 || header.e_version != EV_CURRENT)
    {
        return NULL;
    }
    
    // Allocate some memory for the program headers.
    const uint64_t PHDRS_SIZE = header.e_phnum * header.e_phentsize;
    progHeaders = kmalloc(PHDRS_SIZE);
    char* phdrs_ptr = (char*)progHeaders;

    // Get program headers.
    elf_ptr = orig_elf_ptr + header.e_phoff;
    for (uint64_t i = 0; i < PHDRS_SIZE; ++i)
    {
        phdrs_ptr[i] = elf_ptr[i];
    }

    for (Elf64_Phdr* phdr = progHeaders; (char*)phdr < (char*)progHeaders + header.e_phnum * header.e_phentsize; phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize))
    {
        if (phdr->p_type == PT_LOAD)
        {
            int pages = (phdr->p_memsz + 0x1000 - 1);
            Elf64_Addr segment = phdr->p_paddr;

            for (uint64_t i = segment; i < (segment + pages)*2; i += 0x1000)
            {
                vmm_map_page(pml4, (void*)i, PAGE_P_PRESENT | PAGE_US_USER | PAGE_RW_WRITABLE);
            }

            elf_ptr = orig_elf_ptr + phdr->p_offset;

            char* segment_ptr = (char*)segment;

            for (uint64_t i = 0; i < phdr->p_filesz; ++i)
            {
                segment_ptr[i] = elf_ptr[i];
            }
        }
    }


    void(*entry)(void) = ((__attribute__((sysv_abi))void(*)(void))header.e_entry);
    vmm_map_page(pml4, jmp_to_ring3, PAGE_P_PRESENT | PAGE_US_USER | PAGE_RW_WRITABLE);
    return entry;
}
