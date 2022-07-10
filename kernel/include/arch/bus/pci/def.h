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

#ifndef PCI_DEFINITIONS_H
#define PCI_DEFINITIONS_H


typedef enum 
{
    MASS_STORAGE_CONTROLLER = 0x1,
    NETWORK_CONTROLLER = 0x2,
    DISPLAY_CONTROLLER = 0x3,
    MULTIMEDIA_CONTROLLER = 0x4,
    MEMORY_CONTROLLER = 0x5,
    BRIDGE = 0x6,
} PCI_CLASSCODE;


typedef enum 
{
    SCSI_BUS_CONTROLLER = 0x0,
    IDE_CONTROLLER = 0x1,
    FLOPPY_DISK_CONTROLLER = 0x2,
    IPI_BUS_CONTROLLER = 0x3,
    RAID_CONTROLLER = 0x4,
    ATA_CONTROLLER = 0x5,
    SERIAL_ATA_CONTROLLER = 0x6,
    SERIAL_ATTACHED_SCSI_CONTROLLER = 0x7,
    NON_VOLATILE_MEMORY_CONTROLLER = 0x8
} MASS_STORAGE_SUBCLASS;


typedef enum 
{
    ETHERNET_CONTROLLER = 0x0,
    TOKEN_RING_CONTROLLER = 0x1,
    FDDI_CONTROLLER = 0x2,
    ATM_CONTROLLER = 0x3,
    ISDN_CONTROLLER = 0x4,
    WORD_FIP_CONTROLLER = 0x5,
    PICMG_MULTI_COMPUTING_CONTROLLER = 0x6,
    INFINBAND_CONTROLLER = 0x7,
    FABRIC_CONTROLLER = 0x8
} NETWORK_CONTROLLER_SUBCLASS;


typedef enum
{
    VGA_COMPATIBLE_CONTROLLER = 0x0,
    XGA_CONTROLLER = 0x1,
    _3D_CONTROLLER = 0x2,
} DISPLAY_CONTROLLER_SUBCLASS;


typedef enum 
{
    MULTIMEDIA_VIDEO_CONTROLLER = 0x0,
    MULTIMEDIA_AUDIO_CONTROLLER = 0x1,
    COMPUTER_TELEPHONY_DEVICE = 0x2,
    AUDIO_DEVICE = 0x3
} MULTIMEDIA_CONTROLLER_SUBCLASS;

typedef enum
{
    RAM_CONTROLLER = 0x0,
    FLASH_CONTROLLER = 0x1
} MEMORY_CONTROLLER_SUBCLASS;


typedef enum 
{
    HOST_BRIDGE = 0x0,
    ISA_BRIDGE = 0x1,
    EISA_BRIDGE = 0x2,
    MCA_BRIDGE = 0x3,
    PCI_TO_PCI_BRIDGE = 0x4,
    PCMICIA_BRIDGE = 0x5,
    NUBUS_BRIDGE = 0x6,
    CARDBUS_BRIDGE = 0x7,
    RACEWAY_BRIDGE = 0x8,
} BRIDGE_SUBCLASS;

#endif
