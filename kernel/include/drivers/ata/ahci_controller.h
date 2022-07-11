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

#ifndef AHCI_CONTROLLER_H
#define AHCI_CONTROLLER_H

#include <stdint.h>
#include <drivers/ata/ahci_def.h>

/*
 * @brief   Locates AHCI Host Bus Adapter connected to 
 *          a PCI slot. Returns 1 if found, otherwise 0.
 *
 */

uint8_t ahci_hba_init(void);

/*
 *  @brief  Returns 1 if a sata drive exists, otherwise 0.
 *
 */
uint8_t ahci_sata_exists(void);

/*
 *  @brief  Returns the amount of used ports.
 *
 */
uint8_t ahci_hba_used_ports(void);

struct Drive 
{
    __attribute__((aligned(0x400))) char clb[0x400];        // Command list buffer.
    __attribute__((aligned(0x100))) char fb[0x100];         // FIS buffer.
    __attribute__((aligned(0x80)))  char ctba[0x1F40];      // Command Table Base Address
    HBA_PORT* port;
};

/*
 *  @brief  Gives a drive corresponding to a given drive number.
 *
 */
struct Drive* get_drive(uint8_t driveno);

#endif
