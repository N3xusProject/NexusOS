;; Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
;; See LICENSE file.
bits 64


%define STACK_SIZE 0x1000*3

global jmp_to_ring3
global ring3
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

    push 0x40 | 3
    push rbp                  ;; RSP.
    sti
    pushf
    cli
    push 0x38 | 3
    push 0x2000               ;; RIP (0x2000 is where nexd is loaded in memory).
    sti
    iretq
