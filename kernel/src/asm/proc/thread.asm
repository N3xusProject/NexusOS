;; Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
;; See LICENSE file.

bits 64

global switch_thread
global spawn

extern kprintf
extern current_thread
extern head_thread
extern syscore
extern vmm_alloc_page
extern map_stack
extern kmalloc
extern vmm_mkpml4
extern thread_switch_lock

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
    
    cmp byte [thread_switch_lock], 1
    jge .done

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

    ;; Get next thread and ensure it is not in
    ;; a killed state.
    .find_non_killed:
        nop
        get_thread_get_idx [current_thread], 1
        mov [current_thread], rax
        get_thread_get_idx [current_thread], 2
        cmp rax, THREAD_KILLED
        je .find_non_killed

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
    push 0x40 | 3           ;;SS.
    push rbp
    sti
    pushf
    cli
    push 0x38 | 3

    mov rax, [tmp]
    push rax

    .done:
        sti
        iretq


spawn:
    cli
    ;; Save "where" param.
    push rdi

    ;; We want to check if there is a killed thread
    ;; that we can reclaim so we don't have to allocate
    ;; a new one.

    mov rax, [syscore]
    mov [tmp], rax

    .find_killed:
        get_thread_get_idx [tmp], 1
        mov [tmp], rax

        ;; Are we back at syscore?
        ;; if so, we made a full
        ;; trip around the linked list
        ;; and found no killed threads.
        ;; Thus we must allocate a new thread.
        mov rax, [syscore]
        cmp [tmp], rax
        je .alloc_thread

        get_thread_get_idx [tmp], 2
        cmp rax, THREAD_KILLED
        je .setup_thread

    ;; Allocate a page for a thread control block.
    .alloc_thread:
    call vmm_alloc_page
    mov [tmp], rax
    cmp rax, 0
    je .err

    ;; Set TID as head_thread.TID + 1.
    .setup_thread:
    get_thread_get_idx [head_thread], 0
    inc rax
    set_thread_val_idx [tmp], 0, rax

    ;; Set state as inactive.
    set_thread_val_idx [tmp], 2, THREAD_INACTIVE

    ;; Set next as syscore.
    mov rax, [syscore]
    set_thread_val_idx [tmp], 1, rax

    ;; Allocate a new virtual address space for our thread.
    call vmm_mkpml4
    set_thread_val_idx [tmp], 12, rax

    push rax

    ;; Allocate a stack for our new thread.
    mov rdi, STACK_SIZE
    call kmalloc
    cmp rax, 0
    jz .err

    pop rdi
    mov rsi, rax
    mov rdx, STACK_SIZE
    call map_stack

    add rax, STACK_SIZE
    dec rax
    
    set_thread_val_idx [tmp], 7, rax                    ;; RSP.
    set_thread_val_idx [tmp], 8, rax                    ;; RBP.

    ;; Set RIP for our new thread.
    pop rdi
    set_thread_val_idx [tmp], 11, rdi

    ;; Append our thread to the queue.
    mov rax, [tmp]
    set_thread_val_idx [head_thread], 1, rax
    mov [head_thread], rax

    ;; Return TID.
    get_thread_get_idx [tmp], 0

    retq

    .err:
        mov rax, 0
        retq

section .data
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
