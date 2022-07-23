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

#include <drivers/api/devctl.h>
#include <drivers/ps2/keyb_controller.h>
#include <libkern/driverctl.h>

uint64_t g_devctl_data = 0;


DEVCTL_RESP devctl(DRIVER_CLASS driver_class, DEVCTL_REQ request)
{    
    DEVCTL_RESP(*req_handler)(DEVCTL_REQ) = driverctl_get_reqhandler(driver_class);

    if (req_handler == NULL)
    {
        return DEVCTL_INVALID_DRIVER_CLASS;
    }

    return req_handler(request);
}



uint64_t devctl_in(DRIVER_CLASS driver_class, DEVCTL_REQ request)
{
    g_devctl_data = 0;
    DEVCTL_RESP(*req_handler)(DEVCTL_REQ) = driverctl_get_reqhandler(driver_class);

    // TODO: Somehow set a status.

    req_handler(request);
    return g_devctl_data;
}
