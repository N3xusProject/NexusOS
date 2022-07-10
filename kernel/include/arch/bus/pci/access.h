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

#ifndef PCI_ACCESS_H
#define PCI_ACCESS_H

#define VENDOR_VALID(vendor_id) vendor_id != 0xFFFF

#include <stdint.h>

uint16_t pci_read_vendor_id(uint16_t bus, uint8_t slot, uint8_t func);
uint16_t pci_read_device_id(uint16_t bus, uint8_t slot, uint8_t func);
uint8_t pci_read_class_code(uint16_t bus, uint8_t slot, uint8_t func);
uint8_t pci_read_subclass_code(uint16_t bus, uint8_t slot, uint8_t func);
uint32_t pci_get_bar0(uint16_t bus, uint8_t slot, uint8_t func);
uint32_t pci_get_bar1(uint16_t bus, uint8_t slot, uint8_t func);
uint32_t pci_get_bar2(uint16_t bus, uint8_t slot, uint8_t func);
uint32_t pci_get_bar3(uint16_t bus, uint8_t slot, uint8_t func);
uint32_t pci_get_bar4(uint16_t bus, uint8_t slot, uint8_t func);
uint32_t pci_get_bar5(uint16_t bus, uint8_t slot, uint8_t func);


#endif
