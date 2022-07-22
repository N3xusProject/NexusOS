;; Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
;; See LICENSE file.
bits 64

global syscall_dispatcher
extern syscall_regs
extern syscall_table

%macro set_reg_at 2
    mov r11, syscall_regs
    add r11, 8*%1
    mov qword [r11], %2
%endmacro


;; If changed, change in syscall.c as well.
%define MAX_SYSCALLS 1

syscall_dispatcher:
    cli
    cmp rax, MAX_SYSCALLS
    jge .done

    cmp rax, 0
    jl .done

    set_reg_at 0, rbx
    set_reg_at 1, rcx
    set_reg_at 2, rdx
    set_reg_at 3, rsi
    set_reg_at 4, rdi
    set_reg_at 5, r8
    set_reg_at 6, r9
    set_reg_at 7, r10

    mov r11, syscall_table
    imul rax, 8
    add r11, rax
    call [r11]

    .done:
        sti
        iretq
