/*
 *  Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
 *  See LICENSE file.
*/

#include <arch/bus/pci/access.h>
#include <arch/io/io.h>

#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC

/*
 *  @brief  Reads a word from a device's configuration
 *          space. A configuration space is a set of registers
 *          that are memory mapped. This registers are used for
 *          configuration.
 *
 *
 * @param bus       Bus we want to select.
 * @param slot      Device number we want to select.
 * @param func      A device function we could select.
 * @param offset    Register offset.
 *
 */

static uint16_t pci_config_read_word(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint32_t lbus = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;

    uint32_t config_address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));

    // Write out the address that is required to be
    // accessed.
    outportl(CONFIG_ADDRESS, config_address);

    // Read in configuration-space data.
    tmp = (uint16_t)((inportl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}


uint16_t pci_read_vendor_id(uint8_t bus, uint8_t slot, uint8_t func)
{
    return pci_config_read_word(bus, slot, func, 0);
}


uint16_t pci_read_device_id(uint8_t bus, uint8_t slot, uint8_t func)
{
    return pci_config_read_word(bus, slot, func, 2);
}


uint8_t pci_read_class_code(uint8_t bus, uint8_t slot, uint8_t func)
{
    return pci_config_read_word(bus, slot, func, 0xA) >> 8;
}


uint8_t pci_read_subclass_code(uint8_t bus, uint8_t slot, uint8_t func)
{
    return pci_config_read_word(bus, slot, func, 0xA) & 0xFF;
}


uint32_t pci_get_bar0(uint8_t bus, uint8_t slot, uint8_t func)
{
    uint16_t low_word = pci_config_read_word(bus, slot, func, 0x10);
    uint16_t high_word = pci_config_read_word(bus, slot, func, 0x12);

    return (uint32_t)(high_word << 16 | low_word);
}


uint32_t pci_get_bar1(uint8_t bus, uint8_t slot, uint8_t func)
{
    uint16_t low_word = pci_config_read_word(bus, slot, func, 0x14);
    uint16_t high_word = pci_config_read_word(bus, slot, func, 0x16);

    return (uint32_t)(high_word << 16 | low_word);
}


uint32_t pci_get_bar2(uint8_t bus, uint8_t slot, uint8_t func)
{
    uint16_t low_word = pci_config_read_word(bus, slot, func, 0x18);
    uint16_t high_word = pci_config_read_word(bus, slot, func, 0x1A);

    return (uint32_t)(high_word << 16 | low_word);
}


uint32_t pci_get_bar3(uint8_t bus, uint8_t slot, uint8_t func)
{
    uint16_t low_word = pci_config_read_word(bus, slot, func, 0x1C);
    uint16_t high_word = pci_config_read_word(bus, slot, func, 0x1E);

    return (uint32_t)(high_word << 16 | low_word);
}

uint32_t pci_get_bar4(uint8_t bus, uint8_t slot, uint8_t func)
{
    uint16_t low_word = pci_config_read_word(bus, slot, func, 0x20);
    uint16_t high_word = pci_config_read_word(bus, slot, func, 0x22);

    return (uint32_t)(high_word << 16 | low_word);
}

uint32_t pci_get_bar5(uint8_t bus, uint8_t slot, uint8_t func)
{
    uint16_t low_word = pci_config_read_word(bus, slot, func, 0x24);
    uint16_t high_word = pci_config_read_word(bus, slot, func, 0x26);
    return (uint32_t)(high_word << 16 | low_word);
}
