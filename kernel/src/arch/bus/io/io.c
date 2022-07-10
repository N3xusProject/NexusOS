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

#include <arch/bus/io/io.h>


unsigned char inportb(unsigned short port) {
    unsigned char result;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result;
 }


void outportb(unsigned short port, unsigned char data) {
    __asm__("out %%al, %%dx" : :"a" (data), "d" (port));
}

void outportw(unsigned short port, unsigned short data) {
    __asm__ __volatile__("outw %w0, %w1" : : "a" (data), "Nd" (port));
}

unsigned short inportw(unsigned short port) {
    unsigned short data;
    __asm__ __volatile__("inw %w1, %w0" : "=a" (data) : "Nd" (port));
    return data;
}


void outportl(unsigned short port, uint32_t data) {
    __asm__ __volatile__("outl %0, %w1" : : "a" (data), "Nd" (port));
}


uint32_t inportl(unsigned short port) {
    uint32_t data;
    __asm__ __volatile__("inl %w1, %0" : "=a" (data) : "Nd" (port));
    return data;
}


void io_wait() {
    __asm__("out %%al, %%dx" : :"a" (0x0), "d" (0x80));
}
