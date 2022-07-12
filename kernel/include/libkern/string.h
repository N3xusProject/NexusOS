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




#ifndef STRING_H
#define STRING_H


#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

size_t strlen(const char* str);
bool memcmp(const char* str1, const char* str2, size_t n);
char* dec2str(int dec);
uint8_t* strncpy(uint8_t* dst, const uint8_t* src, const uint8_t len);
uint8_t* hex2str(uint32_t hex_num);
void memzero(void* ptr, size_t n);
void memset(void* ptr, uint64_t data, size_t n);
uint8_t strcmp(const char* str1, const char* str2);
uint8_t strncmp(const char* str1, const char* str2, size_t n);

#endif
