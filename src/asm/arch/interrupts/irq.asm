;;
;; Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
;; See LICENSE file.

bits 64
global isr_irq0
global PIT_TICKS

extern lapic_send_eoi

%macro _pushall 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push rsp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro _popall 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsp
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

;; PIT IRQ.
isr_irq0:
    cli
    _pushall
    call lapic_send_eoi
    inc word [PIT_TICKS]
    _popall
    sti
    iretq


section .data
PIT_TICKS: dw 0         ;; PIT TIMER TICKS.
