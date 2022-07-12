#include <libkern/sleep.h>

extern uint16_t pit_ticks;

void tsleep(uint16_t ticks)
{
    uint32_t e_ticks = pit_ticks + ticks;
    
    while (pit_ticks < e_ticks);
}
