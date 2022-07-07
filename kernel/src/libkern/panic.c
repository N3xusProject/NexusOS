/*
 *  Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
 *  See LICENSE file.
*/

#include <libkern/panic.h>
#include <libkern/log.h>

void panic(void)
{
	// TODO: Do a stack dump or something later.
	__asm__ __volatile__("cli; hlt");
}


void write_panic_msg(void) {
    kprintf(KERN_PANIC);
}
