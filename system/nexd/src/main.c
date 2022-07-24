#include <stdint.h>

int main(void)
{
    __asm__ __volatile__("mov $0, %rax; int $0x80"); 
    
    for (uint32_t x = 50; x < 150; ++x)
    {
        for (uint32_t y  = 50; y < 150; ++y)
        {
            __asm__ __volatile__(
                "\
                mov %0, %%r11;     /* X */ \
                mov %1, %%r10;     /* Y */ \
                mov $0x1, %%rax; \
                mov $0x1, %%rbx; \
                mov $0x20309d850a0, %%rcx; \
                mov %%r11, %%rdx; \
                mov %%r10, %%rsi; \
                mov $0x0000FF, %%rdi; \
                int $0x80" :: "m" (x), "m" (y));
        }
    }

    while (1)
    {


        __asm__ __volatile__(
                "\
                mov $0x2, %rax; /* devctl_in */ \
                mov $0, %rbx;  /* Any Keyboard driver */ \
                mov $0xdeb5a40523c, %rcx; /* Read keystroke */ \
                int $0x80;                 \
                                           \
                andq $0xFFFF, %r9;         \
                cmpq $0x1, %r9;            \
                je .a;                     \
                jmp b;                     \
                                           \
                .a:                        \
                    mov $0x3, %rax;        \
                    int $0x80;             \
                b:                         \
                    nop");
    }
}
