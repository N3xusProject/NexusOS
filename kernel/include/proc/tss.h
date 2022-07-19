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

#ifndef TSS_H
#define TSS_H

#include <stdint.h>

struct TSSEntry {
    uint32_t reserved1;
    uint32_t rsp0Low;
    uint32_t rsp0High;
    uint32_t rsp1Low;
    uint32_t rsp1High;
    uint32_t rsp2Low;
    uint32_t rsp2High;
    uint64_t reserved2;
    uint32_t ist1Low;
    uint32_t ist1High;
    uint32_t ist2Low;
    uint32_t ist2High;
    uint32_t ist3Low;
    uint32_t ist3High;
    uint32_t ist4Low;
    uint32_t ist4High;
    uint32_t ist5Low;
    uint32_t ist5High;
    uint32_t ist6Low;
    uint32_t ist6High;
    uint32_t ist7Low;
    uint32_t ist7High;
    uint64_t reserved3;
    uint16_t reserved4;
    uint16_t io_base;
};

struct __attribute__((packed)) TSSDescriptor {
    uint16_t seglimit;
    uint16_t baseAddrLow;
    uint8_t baseAddrMiddle;
    uint8_t type : 4;
    uint8_t zero : 1;
    uint8_t dpl : 2;
    uint8_t p : 1;
    uint8_t segLimitUpper : 4;
    uint8_t avl : 1;
    uint8_t unused : 2;
    uint8_t g : 1;
    uint8_t baseAddrOtherMiddle;
    uint32_t baseAddrUpper;
    uint32_t reserved;
};

void write_tss(void);
void load_tss(void);

#endif
