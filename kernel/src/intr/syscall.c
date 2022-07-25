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
#include <libkern/elf.h>
#include <libkern/module.h>
#include <drivers/api/devctl.h>
#include <drivers/ps2/keyb_controller.h>
#include <arch/memory/vmm.h>
#include <arch/memory/kheap.h>
#include <proc/thread.h>

// If changed, change the macro in syscall.asm too.
#define MAX_SYSCALLS 6

struct SyscallRegs syscall_regs;

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
    syscall_regs.r9 = devctl(syscall_regs.rbx, syscall_regs.rcx);
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
    syscall_regs.r9 = devctl_in(syscall_regs.rbx, syscall_regs.rcx);
}


static void sys_reboot(void)
{
    send_cpu_reset();
}

/*
 *  @brief  Executes an ELF.
 *
 *  Args:
 *
 *  RBX: path of ELF (has to be loaded at boot).
 *  
 *  Returns 0 in R9 if success, otherwise 1.
 *
 */

static void sys_exec(void)
{
    void(*elf_entry)(void) = elf_get_entry((char*)syscall_regs.rbx);

    if (elf_entry == NULL)
    {
        syscall_regs.r9 = 1;
    }
    else
    {
        spawn(elf_entry);
        syscall_regs.r9 = 0;
    }
}


/*
 *  @brief      Opens a file loaded on boot.
 *
 *  Args: 
 *
 *  RBX: Path.
 *  
 *  Returns pointer to file in R9. NULL if failure. 
 *
 */


static void sys_modopen(void)
{
    struct limine_file* file = get_module_by_name((const char*)syscall_regs.rbx);

    if (file == NULL)
    {
        syscall_regs.r9 = 0;
    }
    else
    {
        extern struct Thread* current_thread;
        uint16_t* ptr = kmalloc(file->size);
        vmm_map_page((void*)current_thread->cr3, ptr, PAGE_P_PRESENT | PAGE_RW_WRITABLE | PAGE_US_USER);
        syscall_regs.r9 = (uint64_t)ptr;

    }
}


void(*syscall_table[MAX_SYSCALLS])(void) = {
    sys_hello,
    sys_devctl,
    sys_devctl_in,
    sys_reboot,
    sys_exec,
    sys_modopen,
};
