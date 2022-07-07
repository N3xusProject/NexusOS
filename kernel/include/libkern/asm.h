/*
 *  Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
 *  See LICENSE file.
*/

#ifndef ASM_H
#define ASM_H

#define CLI __asm__ __volatile__("cli")
#define STI __asm__ __volatile__("sti")
#define HLT  __asm__ __volatile__("hlt")


#endif
