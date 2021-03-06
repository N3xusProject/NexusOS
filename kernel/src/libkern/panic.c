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

#include <libkern/panic.h>
#include <libkern/log.h>
#include <libkern/asm.h>
#include <proc/thread.h>
#include <stdint.h>

extern uint8_t threading_is_init;

void write_panic_msg(void) {
    kprintf(KERN_PANIC);
}

void panic(void)
{
    CLI;
    
    if (!(threading_is_init))
    {
        HLT;
    }

    extern struct Thread* current_thread;
    kprintf("\nKernel panicked while thread %d was running.\n", current_thread->tid);
    kprintf("Thread state dump below:\n");
    kprintf("------------------------------------------------\n");
    kprintf("RAX: %x\nRCX: %x\n", current_thread->rax, current_thread->rcx);
    kprintf("RDX: %x\nRBX: %x\n", current_thread->rdx, current_thread->rbx);
    kprintf("RSP: %x\nRBP: %x\n", current_thread->rsp, current_thread->rbp);
    kprintf("RSI: %x\nRDI: %x\n", current_thread->rsi, current_thread->rdi);
    kprintf("RIP: %x\nCR3: %x\n", current_thread->rip, current_thread->cr3);
    HLT;
}
