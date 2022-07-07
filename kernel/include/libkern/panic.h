/*
 *  Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
 *  See LICENSE file.
*/


#ifndef PANIC_H
#define PANIC_H

#define KERN_PANIC "\033[H\033[2J\033[3J\e[0;31m**KERNEL PANIC**\n"

void panic(void);


#endif
