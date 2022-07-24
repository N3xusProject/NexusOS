#include <stdint.h>

int main(void)
{
    __asm__ __volatile__("mov $0, %rax; int $0x80");

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
