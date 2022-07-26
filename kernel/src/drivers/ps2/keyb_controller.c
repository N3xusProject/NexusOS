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

#include <drivers/ps2/keyb_controller.h>
#include <drivers/api/req.h>
#include <arch/bus/io/io.h>
#include <arch/apic/lapic.h>
#include <libkern/log.h>
#include <stdint.h>

#define BUFFER_SIZE 250


/*
 *  Bits:
 *
 *  Scancode:  15-0
 *  Character: 23-16
 *  Pressed:   24               // 1 if pressed, 0 if released.
 *
 *
 */


__attribute__((section(".bss"))) static uint32_t buffer[BUFFER_SIZE];
static uint16_t keystroke_idx = 0;


static const char* const SC_ASCII = "\x00\x1B" "1234567890-=" "\x08"
    "\x00" "qwertyuiop[]" "\x0D\x1D" "asdfghjkl;'`" "\x00" "\\"
    "zxcvbnm,./" "\x00\x00\x00" " ";

void send_cpu_reset(void) 
{
    outportb(0x64, 0xFE);
}


__attribute__((interrupt)) void irq1_handler(void*)
{
    buffer[keystroke_idx] = inportb(0x60);                 // Read scancode.
    uint8_t ascii = SC_ASCII[buffer[keystroke_idx]];
    uint8_t pressed = !(buffer[keystroke_idx] & 0x80);

    if (keystroke_idx >= BUFFER_SIZE - 1)
    {
        keystroke_idx = 0;
    }

    buffer[keystroke_idx] = (pressed << 24 | ascii << 16 | buffer[keystroke_idx]);
    ++keystroke_idx;
    lapic_send_eoi();
}


DEVCTL_RESP ps2_req_respond(DEVCTL_REQ request)
{
    switch (request)
    {
        case KEYSTROKE_REQ:
            if (keystroke_idx == 0)
            {
                g_devctl_data = 0;
            }
            else
            {
                g_devctl_data = buffer[--keystroke_idx];
            }
            return DEVCTL_OK;
        case FLUSH_KEYSTROKES_REQ:
            keystroke_idx = 0;
            return DEVCTL_OK;
        default:
            return DEVCTL_INVALID_REQUEST;
    }
}
