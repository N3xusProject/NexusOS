;; Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
;; See LICENSE file.

bits 64

global panic

extern current_thread
extern kprintf
extern threading_is_init

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

section .text
panic:    
    cli
    
    ;; Don't dump thread state if threading is not even
    ;; set up.
    cmp byte [threading_is_init], 0
    je .halt

    get_thread_entry [current_thread], 0

    mov rdi, panicmsg1
    mov rsi, rax
    call kprintf

    ;; Dump thread state.
    get_thread_entry [current_thread], 3
    mov rdi, _RAX
    mov rsi, rax
    call kprintf

    get_thread_entry [current_thread], 4
    mov rdi, _RCX
    mov rsi, rax
    call kprintf

    get_thread_entry [current_thread], 5
    mov rdi, _RDX
    mov rsi, rax
    call kprintf

    get_thread_entry [current_thread], 6
    mov rdi, _RBX
    mov rsi, rax
    call kprintf

    get_thread_entry [current_thread], 7
    mov rdi, _RSP
    mov rsi, rax
    call kprintf

    get_thread_entry [current_thread], 8
    mov rdi, _RBP
    mov rsi, rax
    call kprintf

    get_thread_entry [current_thread], 11
    mov rdi, _RIP
    mov rsi, rax
    call kprintf

    .halt:
    hlt

section .data
panicmsg1: db 0xA, "Panic occured while thread %d was running.", 0xA
panicmsg2: db "State of thread below:", 0xA
panicmsg3: db "------------------------------------------", 0xA, 0
_RAX: db "RAX: %x       ", 0
_RCX: db "RAX: %x       ", 0xA, 0
_RDX: db "RDX: %x       ", 0
_RBX: db "RBX: %x       ", 0xA, 0
_RSP: db "RSP: %x   ", 0
_RBP: db "RBP: %x       ", 0xA, 0
_RIP: db "       RIP: %x", 0



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
