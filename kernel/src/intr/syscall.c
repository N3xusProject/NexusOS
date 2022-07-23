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

#include <intr/syscall.h>
#include <libkern/log.h>
#include <drivers/api/devctl.h>
#include <stdint.h>

// If changed, change the macro in syscall.asm too.
#define MAX_SYSCALLS 3


struct SycallRegs 
{
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
} __attribute__((packed)) syscall_regs;


static void sys_hello(void)
{
    kprintf(KINFO "<SYS_HELLO>: Hello from userspace!\n");
}


/*
 *  Args:
 *
 *  RBX: DRIVER_CLASS,
 *  RCX: REQUEST.
 *  
 *  Returns status in RAX.
 *
 *  See the devctl_in documentation in docs/syscalls/devctl.txt
 *
 */

static void sys_devctl(void)
{
    syscall_regs.rax = devctl(syscall_regs.rbx, syscall_regs.rcx);
}


/*
 *  Args:
 *
 *  Same as above except that it also takes
 *  RDX which will be the pointer to a buffer, and RAX returns
 *  a non-status value.
 *  
 *  See the devctl_in documentation in docs/syscalls/devctl_in.txt
 *
 */

static void sys_devctl_in(void)
{
    syscall_regs.rax = devctl_in(syscall_regs.rbx, syscall_regs.rcx);
}


void(*syscall_table[MAX_SYSCALLS])(void) = {
    sys_hello,
    sys_devctl,
    sys_devctl_in,
};
