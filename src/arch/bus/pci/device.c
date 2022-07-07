/*
 *  Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
 *  See LICENSE file.
*/

#include <arch/bus/pci/device.h>
#include <arch/bus/pci/access.h>
#include <libkern/log.h>


// Returns 1 if device is valid, otherwise 0.
static uint8_t verify_device(uint8_t bus, uint8_t slot, uint8_t func)
{
    return pci_read_vendor_id(bus, slot, func) != 0xFFFF;
}


struct PCIDevice pci_locate_device(PCI_CLASSCODE device_class, uint16_t device_subclass)
{
    struct PCIDevice dev;

    for (uint16_t bus = 0; bus < 256; ++bus)
    {
        for (uint8_t slot = 0; slot < 32; ++slot)
        {
            for (uint8_t func = 0; func < 8; ++func)
            {
                uint8_t class_code = pci_read_class_code(bus, slot, func);
                uint8_t subclass_code = pci_read_subclass_code(bus, slot, func);
                if (verify_device(bus, slot, func) && class_code == device_class && subclass_code == device_subclass)
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

    dev.valid = 0;
    return dev;
}
