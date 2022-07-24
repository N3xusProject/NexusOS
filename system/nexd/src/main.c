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


#include <stdint.h>

int main(void)
{
    __asm__ __volatile__("mov $0, %rax; int $0x80"); 
    
    for (uint32_t x = 50; x < 150; ++x)
    {
        for (uint32_t y  = 50; y < 150; ++y)
        {
            __asm__ __volatile__(
                "\
                mov %0, %%r11;     /* X */ \
                mov %1, %%r10;     /* Y */ \
                mov $0x1, %%rax; \
                mov $0x1, %%rbx; \
                mov $0x20309d850a0, %%rcx; \
                mov %%r11, %%rdx; \
                mov %%r10, %%rsi; \
                mov $0x0000FF, %%rdi; \
                int $0x80" :: "m" (x), "m" (y));
        }
    }

    while (1)
    {


        __asm__ __volatile__(
                "\
                mov $0x2, %rax; /* devctl_in */ \
                mov $0, %rbx;  /* Any Keyboard driver */ \
                mov $0xdeb5a40523c, %rcx; /* Read keystroke */ \
                int $0x80;                 \
                                           \
                andq $0xFFFF, %r9;         \
                cmpq $0x1, %r9;            \
                je .a;                     \
                jmp b;                     \
                                           \
                .a:                        \
                    mov $0x3, %rax;        \
                    int $0x80;             \
                b:                         \
                    nop");
    }
}
