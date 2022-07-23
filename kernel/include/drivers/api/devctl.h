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

#ifndef DEVCTL_H
#define DEVCTL_H

#include <stdint.h>
#include <libkern/driverctl.h>

typedef uint64_t DEVCTL_REQ;

typedef enum
{
    DEVCTL_OK,
    DEVCTL_ERR,
    DEVCTL_INVALID_REQUEST,
    DEVCTL_INVALID_DRIVER_CLASS,
} DEVCTL_RESP;

/*
 *  @brief      Exposes an interface for talking with drivers.
 * 
 *  @param request      The request you want to make (example: DEVCTL_ACK_REQ).
 *  @param driver_class  The class of driver you want to talk to (example: DEVCTL_TEST_VDEV).
 *
 *  @returns a response (example: DEVCTL_OK).
 */

DEVCTL_RESP devctl(DRIVER_CLASS driver_class, DEVCTL_REQ request);


#endif
