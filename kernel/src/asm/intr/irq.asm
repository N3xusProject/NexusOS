;; Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
;; See LICENSE file.

bits 64
global irq0
global pit_ticks

extern lapic_send_eoi
extern kprintf
extern switch_thread

irq0:
    cli
    inc word [pit_ticks]
    call lapic_send_eoi
    jmp switch_thread

section .data
pit_ticks: dw 0
