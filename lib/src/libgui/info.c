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

#include <info.h>


uint64_t libgui_get_screen_height(void)
{
    uint64_t height;

    __asm__ __volatile__(
            "\
            mov $0x2, %%rax;  /* sys_devctl_in */ \
            mov $0x1, %%rbx;  /* Framebuffer */   \
            mov $0xbea4b8190d8, %%rcx;            \
            int $0x80; \
            mov %%r9, %0" : "=m" (height));

    return height - 2;
}


uint64_t libgui_get_screen_width(void)
{
    uint64_t width;

    __asm__ __volatile__(
            "\
            mov $0x2, %%rax;  /* sys_devctl_in */ \
            mov $0x1, %%rbx;  /* Framebuffer */   \
            mov $0x40280f00884, %%rcx;            \
            int $0x80; \
            mov %%r9, %0" : "=m" (width));

    return width - 2;
}
