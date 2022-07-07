/*
 *  Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
 *  See LICENSE file.
*/

#include <libkern/driverctl.h>

#define DRIVERCTL_SIZE 1


/*
 *  Each element in this array
 *  represents a hardware device.
 *
 *  Each bit in each element
 *  represents the driver used
 *  for that hardware device.
 *
 */
static uint16_t driverctl[DRIVERCTL_SIZE];

void driverctl_set_used_driver(DRIVER_CLASS driver_class, uint16_t driver_type)
{
    if (driver_class >= DRIVERCTL_SIZE)
    {
        return;
    }

    driverctl[driver_class] |= driver_type;
}


int16_t driverctl_get_used_drivers(DRIVER_CLASS driver_class)
{
    if (driver_class >= DRIVERCTL_SIZE)
    {
        return -1;
    }

    return driverctl[driver_class];
}
