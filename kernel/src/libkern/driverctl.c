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

#include <libkern/driverctl.h>
#include <drivers/ps2/keyb_controller.h>

static DRIVERCTL_TYPE driverctl[DRIVER_CLASS_COUNT];


void driverctl_set_driver(DRIVER_CLASS driver_class, DRIVERCTL_TYPE driver_type)
{
    if (driverctl[driver_class] == 0)
    {
        driverctl[driver_class] = driver_type;
    }
}


DRIVERCTL_TYPE driverctl_get_type(DRIVER_CLASS driver_class)
{
    return driverctl[driver_class];
}


void* driverctl_get_reqhandler(DRIVER_CLASS driver_class)
{
    switch (driverctl_get_type(driver_class))
    {
        case KEYBOARD_TYPE_PS2:
            return ps2_req_respond;
        default:
            return NULL;
    }
}
