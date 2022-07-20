;; Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
;; See LICENSE file.
bits 64

global jmp_to_ring3
global ring3

extern kmalloc

section .text
jmp_to_ring3:
    cli
    ;; Set segment registers.
    mov ax, 0x40 | 3                    ;; User data selector with low bits for RPL.
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push 0x40 | 3
    push 0x500+0x200          ;; RSP.
    pushf
    push 0x38 | 3
    push 0x2000
    iretq

;; Will not be jumped to directly, 
;; instead code will be copied over
;; to a low page.
ring3:
    jmp ring3
    nop                 ;; prepare_ring3() will stop copying here (NOP => OPCODE: 0).
