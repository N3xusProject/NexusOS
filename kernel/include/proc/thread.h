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


#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>

typedef uint64_t TID;
typedef uint16_t TEXIT_REASON;

struct __attribute__((packed)) Thread
{
    TID tid;
    struct Thread* next;
    uint64_t state;
    uint64_t rax;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rbx;
    uint64_t rsp;
    uint64_t rbp;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t rip;
    uint64_t cr3;
};

__attribute__((naked)) void threading_init(void);

/*
 *  @brief: Puts the current thread into a killed state.
 *          whenever ANY process get's killed, a return code
 *          is stored in the thread's RAX.
 *
 */
__attribute__((naked)) void exit(TEXIT_REASON errno);

/*
 *  @brief      Spawns a new thread.
 *
 *  @param where        Location for thread to start executing.
 *
 *  @returns            TID of new thread, zero if error (i.e no mem left).
 */

TID spawn(void* where);

#endif
