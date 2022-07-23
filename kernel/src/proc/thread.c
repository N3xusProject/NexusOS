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

#include <proc/thread.h>
#include <arch/memory/vmm.h>
#include <arch/memory/kheap.h>
#include <arch/memory/mem.h>
#include <libkern/asm.h>
#include <libkern/log.h>
#include <libkern/panic.h>

#define THREAD_ACTIVE 0
#define THREAD_INACTIVE 1
#define THREAD_KILLED 2
#define STACK_SIZE 0x1000*3

struct Thread* current_thread;
struct Thread* syscore;
struct Thread* head_thread;
static uint64_t rip = 0;
uint8_t threading_is_init = 0;             // > 0 if threading is enabled.

void load_pml4(void*);


__attribute__((naked)) void exit(TEXIT_REASON errno)
{
    CLI;

    if (current_thread->tid == 1)
    {
        kprintf(KERN_PANIC "Critical system process terminated: syscore\n");
        panic();
    }
        
    current_thread->state = THREAD_KILLED;
    current_thread->rax = errno;

    // Wait until thread switch.
    __asm__ __volatile__(
            "sti;       \
            wait:       \
                nop;    \
                jmp wait");
}



__attribute__((naked)) void threading_init(void)
{  
    threading_is_init = 1; 
    // -- Load new address space --
    void* new_vaddrsp = vmm_mkpml4();
    load_pml4(new_vaddrsp);

    syscore = vmm_alloc_page();
    current_thread = syscore;
    head_thread = syscore;

    syscore->tid = 1;
    syscore->next = syscore;
    syscore->state = THREAD_ACTIVE;
    syscore->rsp = (uint64_t)kmalloc(STACK_SIZE);

    syscore->rsp += STACK_SIZE - 1;
    syscore->rbp = syscore->rsp;

    // -- Fetch instruction pointer --
    __asm__ __volatile__(
            "\
            movq (%%rsp), %%rax; \
            movq %%rax, %0" : "=r" (rip));

    // -- Setup new stack --
    __asm__ __volatile__(
            "\
            movq %0, %%rsp; \
            movq %0, %%rbp;" :: "r" (syscore->rsp));


    syscore->rip = rip;
    
    vmm_map_page(new_vaddrsp, (void*)rip,  PAGE_P_PRESENT | PAGE_RW_WRITABLE);
    __asm__ __volatile__("movq %0, (%%rsp); retq" :: "r" (rip));
}


void map_stack(uint64_t pml4, uint64_t stack_base, uint64_t n_bytes)
{
    for (uint64_t i = 0; i < n_bytes*3; i += 0x1000)
    {
        vmm_map_page((void*)pml4, (void*)stack_base, PAGE_P_PRESENT | PAGE_RW_WRITABLE | PAGE_US_USER);
        stack_base += 0x1000; 
    }
}
