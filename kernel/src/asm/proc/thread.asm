;; Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
;; See LICENSE file.

bits 64

global switch_thread
global threading_init
global current_thread

extern kprintf
extern vmm_alloc_page

%define PAGE_SIZE 0x1000

;; To get TID do this:
;; get_thread_entry [thread], 0
;; arg 1 is thread pointer.
;; arg 2 is index.
;; 8 bytes per inex.
%macro get_thread_entry 2
    mov rax, %1
    add rax, 8*%2
    mov rax, [rax]
%endmacro

;; Same as above but 3rd arg 
;; is value to set.
%macro set_thread_entry 3
    mov rax, %1
    add rax, 8*%2
    mov rdx, %3
    mov [rax], rdx
%endmacro

switch_thread:
    ;; Save current threads state.
    set_thread_entry [current_thread], 3, rax
    set_thread_entry [current_thread], 4, rcx
    set_thread_entry [current_thread], 5, rdx
    set_thread_entry [current_thread], 6, rbx
    set_thread_entry [current_thread], 7, rsp
    set_thread_entry [current_thread], 8, rbp
    set_thread_entry [current_thread], 9, rsi
    set_thread_entry [current_thread], 10, rdi
    set_thread_entry [current_thread], 11, [rsp]
    mov rbx, cr3
    set_thread_entry [current_thread], 12, rbx

    ;; Get next thread.
    get_thread_entry [current_thread], 1
    mov [current_thread], rax

    ;; Load thread's state.
    get_thread_entry [current_thread], 3          ;; Puts value of thread's RAX into RAX.

    get_thread_entry [current_thread], 4
    mov rcx, rax

    get_thread_entry [current_thread], 5
    mov rdx, rax

    get_thread_entry [current_thread], 6
    mov rbx, rax

    get_thread_entry [current_thread], 7
    mov rsp, rax

    get_thread_entry [current_thread], 8
    mov rbp, rax

    get_thread_entry [current_thread], 9
    mov rsi, rax

    get_thread_entry [current_thread], 10
    mov rdi, rax

    get_thread_entry [current_thread], 12
    mov cr3, rax

    get_thread_entry [current_thread], 0

    mov rdi, msg
    mov rsi, rax
    call kprintf

    mov [rsp - 3], rsp          ;; Set IRET stackframe RSP.
    get_thread_entry [current_thread], 11
    mov [rsp], rax              ;; Set IRET stackframe RIP.
    
    sti
    iretq

threading_init:
    ;; Save return location.
    mov rax, [rsp]
    mov [tmp], rax

    ;; Allocate a page for syscore.
    mov rdi, 1 << 0 | 1 << 1        ;; PAGE_P_PRESENT | PAGE_RW_WRITABLE
    call vmm_alloc_page
    mov [syscore], rax
    
    set_thread_entry syscore, 0, 1              ;; Syscore's TID to 1.
    set_thread_entry syscore, 1, syscore        ;; Set syscore's next to syscore.

    ;; Allocate a page for syscore stack.
    mov rdi, 1 << 0 | 1 << 1        ;; PAGE_P_PRESENT | PAGE_RW_WRITABLE
    call vmm_alloc_page
    mov rbx, rax

    ;; Set syscore stack.
    add rbx, PAGE_SIZE
    dec rbx
    set_thread_entry syscore, 7, rbx

    ;; Change stacks.
    ; mov rsp, rbx
    ; mov rbp, rbx

    ;; Push return location.
    ; push qword [tmp]

    mov rax, syscore
    mov [current_thread], rax

    retq

section .data
msg: db "Thread %d up and running.", 0xA, 0
current_thread: dq 0
tmp: dq 0
syscore: dq 0

;; Thread structure layout.
;; qword tid            ;; Thread ID.
;; qword next           ;; Next thread.
;; qword state          ;; Current state.
;; qword rax
;; qword rcx
;; qword rdx
;; qword rbx
;; qword rsp
;; qword rbp
;; qword rsi
;; qword rdi
;; qword rip
;; qword cr3
