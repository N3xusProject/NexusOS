;; Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
;; See LICENSE file.
bits 64

global jmp_to_ring3
global ring3

%define TMP_USER_STACK_START 0x500+0x200

extern ring3_entry

section .text
jmp_to_ring3:
    cli
    ;; Set segment registers.
    mov ax, 0x40 | 3                    ;; User data selector with low bits for RPL.
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov rbp, TMP_USER_STACK_START

    push 0x40 | 3
    push rbp                  ;; RSP.
    pushf
    push 0x38 | 3
    push 0x2000               ;; RIP (0x2000 is where nexd is loaded in memory).
    iretq
