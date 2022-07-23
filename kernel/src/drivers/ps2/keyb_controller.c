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
#include <arch/bus/io/io.h>

#define CAPSLOCK_LED (1 << 2)

static const char* const SC_ASCII = "\x00\x1B" "1234567890-=" "\x08"
    "\x00" "qwertyuiop[]" "\x0D\x1D" "asdfghjkl;'`" "\x00" "\\"
    "zxcvbnm,./" "\x00\x00\x00" " ";

void send_cpu_reset(void) 
{
    outportb(0x64, 0xFE);
}


static void toggle_capslock_led(void)
{
    static uint8_t on = 0;
    uint8_t attempts = 0;

    outportb(0x64, 0xED);
    while (inportb(0x64) & (1 << 1));

    outportb(0x60, CAPSLOCK_LED);
}


DEVCTL_RESP ps2_send_req(DEVCTL_REQ request)
{
    switch (request)
    {
        case CAPSLOCK_LED_TOGGLE_REQ:
            toggle_capslock_led();
            return DEVCTL_OK;
        default:
            return DEVCTL_INVALID_REQUEST;
    }
}
