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

#include <arch/bus/pci/device.h>
#include <arch/bus/pci/access.h>
#include <libkern/log.h>

// Returns 1 if device is valid, otherwise 0.
static uint8_t verify_device(uint8_t bus, uint8_t slot, uint8_t func)
{
    return pci_read_vendor_id(bus, slot, func) != 0xFFFF;
}

struct PCIDevice pci_device_lookup(PCI_CLASSCODE classcode, uint16_t subclass)
{ 
    for (uint16_t bus = 0; bus < 256; ++bus)
    {
        for (uint8_t slot = 0; slot < 32; ++slot)
        {
            for (uint8_t func = 0; func < 8; ++func)
            {
                uint8_t device_class = pci_read_class_code(bus, slot, func);
                uint8_t device_subclass = pci_read_subclass_code(bus, slot, func);
                uint8_t prog_if = pci_read_prog_if(bus, slot, func);
                if (verify_device(bus, slot, func) && device_class == classcode && device_subclass == subclass && prog_if == 1)
                {
                    struct PCIDevice dev = {
                        .valid = 1,
                        .bus = bus,
                        .slot = slot,
                        .func = func
                    };

                    return dev;
                }
            }
        }
    }
    struct PCIDevice dev = {
        .valid = 0
    };

    return dev;
}
