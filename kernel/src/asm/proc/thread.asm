;; Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
;; See LICENSE file.

bits 64

global switch_thread
global threading_init
global current_thread
global threading_is_init
global syscore
global get_rip
global spawn
global a
global b
global c

extern kprintf
extern vmm_alloc_page
extern kmalloc
extern vmm_mkpml4

%define PAGE_SIZE 0x1000
%define STACK_SIZE 0x1000*3
%define THREAD_ACTIVE 0
%define THREAD_INACTIVE 1
%define THREAD_KILLED 2

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

get_rip:
    pop rax
    jmp rax

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
    set_thread_entry [current_thread], 12, cr3

    ;; Set current thread to inactive.
    set_thread_entry [current_thread], 2, THREAD_INACTIVE

    ;; Get next thread.
    .get_next_thread:
        get_thread_entry [current_thread], 1
        mov [current_thread], rax
        get_thread_entry [current_thread], 2
        cmp rax, THREAD_KILLED
        je .get_next_thread

    ;; Set current thread to active.
    set_thread_entry [current_thread], 2, THREAD_ACTIVE

    ;; Load thread's state.
    get_thread_entry [current_thread], 3          ;; Puts value of thread's RAX into RAX.

    get_thread_entry [current_thread], 4
    mov rcx, rax

    get_thread_entry [current_thread], 5
    mov rdx, rax

    get_thread_entry [current_thread], 6
    mov rbx, rax

    get_thread_entry [current_thread], 12
    mov cr3, rax

    get_thread_entry [current_thread], 7
    mov rsp, rax

    get_thread_entry [current_thread], 8
    mov rbp, rax

    get_thread_entry [current_thread], 9
    mov rsi, rax

    get_thread_entry [current_thread], 10
    mov rdi, rax

    get_thread_entry [current_thread], 0

    mov rdi, msg
    mov rsi, rax
    call kprintf
    
    ;; Get rid of IRET stack frame.
    times 5 pop rax

    ;; Set return location.
    get_thread_entry [current_thread], 11
    mov [rsp], rax
    
    sti
    retq

threading_init:
    ;; Save return location.
    mov rax, [rsp]
    mov [tmp], rax

    ;; Allocate a page for syscore.
    mov rdi, 1 << 0 | 1 << 1        ;; PAGE_P_PRESENT | PAGE_RW_WRITABLE
    call vmm_alloc_page
    mov [syscore], rax
    
    set_thread_entry [syscore], 0, 1              ;; Syscore's TID to 1.
    set_thread_entry [syscore], 1, [syscore]      ;; Set syscore's next to syscore.

    ;; Allocate a page for syscore stack.
    ; mov rdi, 1 << 0 | 1 << 1        ;; PAGE_P_PRESENT | PAGE_RW_WRITABLE
    ; call vmm_alloc_page
    ; mov rbx, rax

    mov rdi, STACK_SIZE
    call kmalloc
    mov rbx, rax

    ;; Set syscore stack.
    add rbx, STACK_SIZE
    dec rbx
    set_thread_entry [syscore], 7, rbx
    set_thread_entry [syscore], 8, rbx

    ;; Change stacks.
    mov rsp, rbx
    mov rbp, rbx

    ;; Set current thread's RIP to return location.
    mov rbx, [tmp]
    set_thread_entry [syscore], 11, rbx

    ;; Set current thread's state to active.
    set_thread_entry [syscore], 2, THREAD_ACTIVE

    ;; Push return location.
    push qword [tmp]

    ;; Set current thread.
    mov rax, [syscore]
    mov [current_thread], rax

    ;; Set head.
    mov [head], rax

    ;; Set init byte.
    mov byte [threading_is_init], 1

    retq


spawn:
    cli

    push rdi
    ;; Allocates a page for a new thread.
    mov rdi, 1 << 0 | 1 << 1        ;; PAGE_P_PRESENT | PAGE_RW_WRITABLE
    call vmm_alloc_page
    mov [tmp], rax

    cmp rax, 0
    je .done

    ;; Set new thread's TID to head's TID + 1.
    get_thread_entry [head], 0
    mov rbx, rax
    inc rbx
    set_thread_entry [tmp], 0, rbx

    ;; Allocate a page for new thread's stack.
    mov rdi, 1 << 0 | 1 << 1        ;; PAGE_P_PRESENT | PAGE_RW_WRITABLE
    call vmm_alloc_page
    cmp rax, 0
    je .done
    mov rbx, rax

    ;; Setup new thread's stack.
    add rbx, PAGE_SIZE
    dec rbx
    set_thread_entry [tmp], 7, rbx
    set_thread_entry [tmp], 8, rbx

    ;; Set new thread's next to syscore.
    set_thread_entry [tmp], 1, [syscore]

    ;; Set new thread's state to inactive.
    set_thread_entry [tmp], 2, THREAD_INACTIVE

    ;; Make a new virtual address space.
    call vmm_mkpml4
    
    ;; Set new thread's virtual address space.
    set_thread_entry [tmp], 12, cr3

    ;; Set new thread's RIP.
    pop rdi
    set_thread_entry [tmp], 11, rdi

    ;; Add new thread to queue.
    set_thread_entry [head], 1, [tmp]
    mov rdx, [tmp]
    mov [head], rdx 

    .done:
    sti
    retq

a:
    times 5 nop
    hlt
    jmp a

b:
    times 5 nop
    hlt
    jmp b

c:
    times 5 nop
    hlt
    jmp c

section .data
msg: db "Thread %d is running.", 0xA, 0
current_thread: dq 0
tmp: dq 0
syscore: dq 0
head: dq 0
threading_is_init: db 0              ;; 1 if threading is enabled.

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
