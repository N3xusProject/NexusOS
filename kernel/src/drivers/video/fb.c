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

#include <drivers/video/fb.h>
#include <drivers/api/req.h>
#include <proc/mutex.h>
#include <libkern/log.h>
#include <arch/memory/vmm.h>
#include <limine.h>

static struct limine_framebuffer_request fb_req = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

static MUTEXLOCK mutex = 0;
static uint32_t* addr;
static uint16_t bpp;                        // Bits per pixel.
static struct limine_framebuffer* fb;
static uint32_t* back_buffer;

#define BACKBUF_SIZE (fb->pitch * fb->height)


uint64_t fb_get_width(void)
{
    return fb->width;
}


uint64_t fb_get_height(void)
{
    return fb->height;
}


static void fb_swap_buffers(void)
{
    for (uint64_t i = 0; i < BACKBUF_SIZE / 4; ++i)
    {
        addr[i] = back_buffer[i];
    }
}

void fbdriver_init(void)
{
    fb = fb_req.response->framebuffers[0];
    addr = fb->address;
    bpp = fb->bpp;


    for (uint64_t i = 0; i < BACKBUF_SIZE; i += 0x1000)
    {
        if (i == 0)
        {
            back_buffer = vmm_alloc_page();
            continue;
        }

        vmm_alloc_page();
    }
}


void put_pix(uint32_t x, uint32_t y, uint32_t color)
{
    // Ensure the x and y are not out of bounds.
    if (x > fb->width - 1 || y > fb->height - 1)
    {
        return;
    }

    uint32_t pixel_idx = x + y * fb->width;
    back_buffer[pixel_idx] = color;
}


void fb_clear(uint32_t color)
{
    mutex_lock(&mutex);
    for (uint32_t x = 0; x < fb->width; ++x)
    {
        for (uint32_t y = 0; y < fb->height; ++y)
        {
            put_pix(x, y, color);
        }
    }

    mutex_unlock(&mutex);
}


DEVCTL_RESP fb_req_respond(DEVCTL_REQ request)
{

    switch (request)
    {
        case FB_PIXEL_PLACEMENT_REQ:
            put_pix(syscall_regs.rdx, syscall_regs.rsi, syscall_regs.rdi);
            return DEVCTL_OK;
        case FB_GET_WIDTH:
            g_devctl_data = fb_get_width();
            break;
        case FB_GET_HEIGHT:
            g_devctl_data = fb_get_height();
            break;
        case FB_SWAP_BUFFERS:
            fb_swap_buffers();
        default:
            return DEVCTL_INVALID_REQUEST;
    }
}
