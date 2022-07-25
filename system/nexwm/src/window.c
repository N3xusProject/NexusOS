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

#include <window.h>
#include <libgui/draw.h>

#define MAX_WINDOW_WIDTH 500
#define MAX_WINDOW_HEIGHT 800
#define DEFAULT_WINDOW_COLOR 0x808080


uint8_t open_window(uint32_t x, uint32_t y, uint16_t width, uint16_t height, const char* title)
{
    if (height > MAX_WINDOW_HEIGHT || width > MAX_WINDOW_WIDTH)
    {
        return 1;
    }

    // Draw the window body.
    libgui_draw_square(x, y, width, height, 0x808080);
    libgui_draw_square(x, y, width, height, 0x808080);

    // Draw the top bar.
    libgui_draw_square(x, y, width, 17, 0x000000);
    libgui_putstr(x + 2, y, 0xFFFFFF, title);

    libgui_bufswap();

    return 0;
}
