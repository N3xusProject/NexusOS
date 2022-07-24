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

#ifndef DRIVERCTL_H
#define DRIVERCTL_H


/*
 *  Module description: Allows a way for managing drivers.
 *
 *                      A driver class could be seen as 
 *                      a type of device.
 *
 *                      There could be a driver class of HDD
 *                      which would have an enum corresponding 
 *                      to it (DRIVER_HDD_TYPE) which will have a 
 *                      list of drivers that could be used for that specific
 *                      device (i.e driver class).
 *
 */


#include <stdint.h>
#include <stddef.h>

#define DRIVER_CLASS_COUNT 2

typedef enum
{
    DRIVERCLASS_KEYBOARD,
    DRIVERCLASS_FRAMEBUFFER
} DRIVER_CLASS;

typedef enum
{
    // Keyboard types.
    KEYBOARD_TYPE_PS2 = 1,

    // Framebuffer types.
    FRAMEBUFFER_DRIVER_TYPE_DEFAULT = 2,
} DRIVERCTL_TYPE;


/*
 *  @brief  Sets a driver that will be used
 *          for a driver class.
 *
 */
void driverctl_set_driver(DRIVER_CLASS driver_class, DRIVERCTL_TYPE driver_type);

/*
 *  @brief  Gets driver type from driver class.
 *
 */
DRIVERCTL_TYPE driverctl_get_type(DRIVER_CLASS driver_class);

/*
 *  @brief  Returns a pointer to the function
 *          that handles requests for the currently
 *          in use driver class.
 *
 *          Returns NULL if no request handler
 *          was found.
 *
 */

void* driverctl_get_reqhandler(DRIVER_CLASS driver_class);

#endif
