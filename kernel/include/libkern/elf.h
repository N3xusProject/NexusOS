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

#ifndef ELF_WRAPPER_H
#define ELF_WRAPPER_H

#include <stdint.h>
#include <elf.h>

#define EXEC_PATH_NOT_FOUND 1
#define EXEC_ELF_HEADER_BAD 2


/*
 *  @brief  Allocates memory for an entry point
 *          and returns ELF entry point by a module path.
 *
 *
 * @returns entry to elf, otherwise NULL if failure.
 * 
 * @param ring3_exec    If 1, the ELF will execute in ring 3.
 *
 */
void* elf_get_entry(const char* path);

#endif
