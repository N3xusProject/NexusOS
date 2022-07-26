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


#ifndef DEVCTL_REQ_H
#define DEVCTL_REQ_H

// NOTE: Request IDs are 11 hex digits.


// PS2 KEYBOARD CONTROLLER DRIVER REQUESTS
#define KEYSTROKE_REQ 0xdeb5a40523c
#define FLUSH_KEYSTROKES_REQ 0x8cb5e3a0cb9

// FRAMEBUFFER DRIVER REQUESTS
#define FB_PIXEL_PLACEMENT_REQ 0x20309d850a0
#define FB_GET_WIDTH 0x40280f00884
#define FB_GET_HEIGHT 0xbea4b8190d8
#define FB_SWAP_BUFFERS 0x20cad5a8913
#define FB_PUTCHAR 0x5bcff5320c9


#endif
