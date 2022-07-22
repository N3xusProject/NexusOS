__attribute__((naked)) int main(void)
{
    __asm__ __volatile__("mov $0x0, %rax; int $0x80");
    while (1);
}
