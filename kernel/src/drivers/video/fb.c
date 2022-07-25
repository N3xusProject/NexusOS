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
#include <libkern/panic.h>
#include <libkern/module.h>
#include <arch/memory/vmm.h>
#include <arch/memory/kheap.h>
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
static struct limine_file* font_file;


#define BACKBUF_SIZE (fb->pitch * fb->height)

struct PSF1_FONT_HEADER 
{
    unsigned char magic[2];
    unsigned char mode;
    unsigned char chsize;
};

struct PSF1_FONT 
{
    void* glyphBuffer;
};


static struct PSF1_FONT_HEADER* builtin_font_header;
static struct PSF1_FONT builtin_font;


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


static void init_builtin_font(void)
{
    // First, load the font.
    font_file = get_module_by_name("/assets/font1.psf");
 
    if (font_file == NULL)
    {
        kprintf(KERN_PANIC "Could not load builtin font!\n");
        panic();
    }

    builtin_font_header = kmalloc(sizeof(struct PSF1_FONT_HEADER));

    if (builtin_font_header == NULL)
    {
        kprintf(KERN_PANIC "Failed to allocate memory for builtin font's header.\n");
        panic();
    }

    int8_t* font_data = (uint8_t*)font_file->address;

    // To read data into the header.
    uint8_t* header_ptr = (uint8_t*)builtin_font_header;

    // Read font header into our allocated memory.
    for (uint16_t i = 0; i < sizeof(struct PSF1_FONT_HEADER); ++i)
    {
        header_ptr[i] = font_data[i];
    }

    // Get glyph buffer size and allocate memory for glpyh buffer.
    uint64_t glyph_buffer_size = builtin_font_header->chsize * 256;

    if (builtin_font_header->mode == 1)
    {
        glyph_buffer_size = builtin_font_header->chsize * 512;
    }

    builtin_font.glyphBuffer = kmalloc(glyph_buffer_size);
    
    char* glyphBufferPtr = (char*)builtin_font.glyphBuffer;

    // Read-in glyphs.
    for (uint64_t i = sizeof(struct PSF1_FONT_HEADER), j = 0; i < sizeof(struct PSF1_FONT_HEADER) + glyph_buffer_size; ++i, ++j)
    {
        glyphBufferPtr[j] = font_data[i];
    }
}

static void put_char(uint32_t x, uint32_t y, uint32_t color, char c) {
    char* fontPtr = builtin_font.glyphBuffer + (c * builtin_font_header->chsize);
    for (uint64_t cy = y; cy < y + 12; ++cy) {
        for (uint64_t cx = x; cx < x + 8; ++cx) {
            if (cy < y+2) continue;     // To remove artifacts on top of letter.

            if ((*fontPtr & ((1 << 7) >> (cx - x))) > 0) {
                put_pix(cx, cy, color);
            }

        }

        ++fontPtr;
    }
}


void fbdriver_init(void)
{
    init_builtin_font();
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
            return DEVCTL_OK;
        case FB_GET_HEIGHT:
            g_devctl_data = fb_get_height();
            return DEVCTL_OK;
        case FB_SWAP_BUFFERS:
            fb_swap_buffers();
            return DEVCTL_OK;
        case FB_PUTCHAR:
            put_char(syscall_regs.rdx, syscall_regs.rsi, syscall_regs.rdi, syscall_regs.r8);
            return DEVCTL_OK;
        default:
            return DEVCTL_INVALID_REQUEST;
    }
}
