;; Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
;; See LICENSE file.

bits 64

global switch_thread
global threading_is_init
global spawn

extern kprintf
extern current_thread
extern head_thread
extern syscore
extern vmm_alloc_page
extern kmalloc
extern vmm_mkpml4

%define PAGE_SIZE 0x1000
%define STACK_SIZE 0x1000*3

%define THREAD_ACTIVE 0
%define THREAD_INACTIVE 1
%define THREAD_KILLED 2

;; Set a value in the thread struct
;; at a certain index. (8 bytes per index).
;;
;; arg 1: thread.
;; arg 2: index.
;; arg 3: value.
%macro set_thread_val_idx 3
    mov r11, %1
    add r11, 8*%2
    mov qword [r11], %3
%endmacro

;; Same as above except there is no third argument 
;; and a value is returned in RAX.
%macro get_thread_get_idx 2
    mov r11, %1
    add r11, 8*%2
    mov rax, [r11]
%endmacro

switch_thread:
    cli
    ;; Save thread state.
    ;; Save RIP.
    mov rax, [rsp]
    mov [tmp], rax

    set_thread_val_idx [current_thread], 2, THREAD_INACTIVE
    set_thread_val_idx [current_thread], 3, rax
    set_thread_val_idx [current_thread], 4, rcx
    set_thread_val_idx [current_thread], 5, rdx
    set_thread_val_idx [current_thread], 6, rbx
    set_thread_val_idx [current_thread], 7, rsp
    set_thread_val_idx [current_thread], 8, rbp
    set_thread_val_idx [current_thread], 9, rsi
    set_thread_val_idx [current_thread], 10, rdi
    mov rax, [tmp]
    set_thread_val_idx [current_thread], 11, rax
    mov rax, cr3
    set_thread_val_idx [current_thread], 12, rax

    ;; Get next thread.
    get_thread_get_idx [current_thread], 1
    mov [current_thread], rax

    ;; Set this thread as active.
    set_thread_val_idx [current_thread], 2, THREAD_ACTIVE

    ;; Restore thread state.
    get_thread_get_idx [current_thread], 3
    get_thread_get_idx [current_thread], 4
    mov rcx, rax

    get_thread_get_idx [current_thread], 5
    mov rdx, rax

    get_thread_get_idx [current_thread], 6
    mov rbx, rax

    get_thread_get_idx [current_thread], 7
    mov rsp, rax

    get_thread_get_idx [current_thread], 8
    mov rbp, rax

    get_thread_get_idx [current_thread], 9
    mov rsi, rax

    get_thread_get_idx [current_thread], 10
    mov rdi, rax

    get_thread_get_idx [current_thread], 11
    mov [tmp], rax

    get_thread_get_idx [current_thread], 12
    mov cr3, rax

    times 5 pop rax
    mov rax, [tmp]
    mov [rsp], rax

    sti
    retq


spawn:
    cli
    ;; Save "where" param.
    push rdi

    ;; Allocate a page for a thread control block.
    call vmm_alloc_page
    mov [tmp], rax

    ;; Set TID as head_thread.TID + 1.
    get_thread_get_idx [head_thread], 0
    inc rax
    set_thread_val_idx [tmp], 0, rax

    ;; Set state as inactive.
    set_thread_val_idx [tmp], 2, THREAD_INACTIVE

    ;; Set next as syscore.
    mov rax, [syscore]
    set_thread_val_idx [tmp], 1, rax

    ;; Allocate a stack for our new thread.
    mov rdi, STACK_SIZE
    call kmalloc

    add rax, STACK_SIZE
    dec rax

    set_thread_val_idx [tmp], 7, rax                    ;; RSP.
    set_thread_val_idx [tmp], 8, rax                    ;; RBP.

    ;; Allocate a new virtual address space for our thread.
    call vmm_mkpml4
    set_thread_val_idx [tmp], 12, rax

    ;; Set RIP for our new thread.
    pop rdi
    set_thread_val_idx [tmp], 11, rdi

    ;; Append our thread to the queue.
    mov rax, [tmp]
    set_thread_val_idx [head_thread], 1, rax
    mov [head_thread], rax

    ;; Return TID.
    get_thread_get_idx [tmp], 0

    sti
    retq

section .data
threading_is_init: db 0              ;; 1 if threading is enabled.
tmp: dq 0

;; Thread structure layout.
;; 0  - qword tid            ;; Thread ID.
;; 1  - qword next           ;; Next thread.
;; 2  - qword state          ;; Current state.
;; 3  - qword rax
;; 4  - qword rcx
;; 5  - qword rdx
;; 6  - qword rbx
;; 7  - qword rsp
;; 8  - qword rbp
;; 9  - qword rsi
;; 10 - qword rdi
;; 11 - qword rip
;; 12 - qword cr3
