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


%macro get_reg_at 1
    mov r11, syscall_regs
    add r11, 8*%1
    mov rax, qword [r11]
%endmacro


;; If changed, change in syscall.c as well.
%define MAX_SYSCALLS 2

syscall_dispatcher:
    cli
    cmp rax, MAX_SYSCALLS
    jge .done

    cmp rax, 0
    jl .done
    
    set_reg_at 0, rax
    set_reg_at 1, rbx
    set_reg_at 2, rcx
    set_reg_at 3, rdx
    set_reg_at 4, rsi
    set_reg_at 5, rdi
    set_reg_at 6, r8
    set_reg_at 7, r9
    set_reg_at 8, r10

    mov r11, syscall_table
    imul rax, 8
    add r11, rax
    call [r11]

    get_reg_at 0

    get_reg_at 1
    mov rbx, rax


    get_reg_at 2
    mov rcx, rax


    get_reg_at 3
    mov rdx, rax


    get_reg_at 4
    mov rsi, rax


    get_reg_at 5
    mov rdi, rax


    get_reg_at 6
    mov r8, rax


    get_reg_at 7
    mov r9, rax


    get_reg_at 8
    mov r10, rax

    .done:
        sti
        iretq
