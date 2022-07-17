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
#include <arch/memory/mem.h>
#include <libkern/asm.h>
#include <libkern/log.h>
#include <libkern/panic.h>

#define THREAD_ACTIVE 0
#define THREAD_INACTIVE 1
#define THREAD_KILLED 2

extern volatile struct Thread* current_thread;
extern volatile struct Thread* syscore;


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

