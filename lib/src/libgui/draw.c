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

#include <draw.h>


void libgui_draw_square(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color)
{
    for (uint32_t sx = x; sx < x + width; ++sx)
    {
        for (uint32_t sy = y; sy < y + height; ++sy)
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
                mov %2, %%rdi; \
                int $0x80" :: "m" (sx), "m" (sy), "m" (color));
        }
    }
}


void libgui_bufswap(void)
{
    __asm__ __volatile__(
            "mov $0x1, %rax; \
            mov $0x1, %rbx; \
            mov $0x20cad5a8913, %rcx; \
            int $0x80");
}


void libgui_putchar(uint32_t x, uint32_t y, uint32_t color, char ch)
{
    __asm__ __volatile__(
            "mov $0x1, %%rax; \
            mov $0x1, %%rbx; \
            mov $0x5bcff5320c9, %%rcx; \
            mov %0, %%rdx; \
            mov %1, %%rsi; \
            mov %2, %%rdi; \
            mov %3, %%r8; \
            int $0x80" :: "m" (x),
                          "m" (y),
                          "m" (color),
                          "m" (ch));
}


void libgui_putstr(uint32_t x, uint32_t y, uint32_t color, const char* str)
{
    uint64_t str_length = 0;
    while (str[str_length++]);

    for (uint32_t i = 0; i < str_length - 1; ++i)
    {
        libgui_putchar(x, y, color, *str++);
        x += 10;
    }
}
