;;
;; Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
;; See LICENSE file.


bits 64

global divide_error
global debug_exception
global overflow
global bound_range_exceeded
global invalid_opcode
global no_mathcoprocessor
global double_fault
global invalid_tss
global segment_not_present
global stack_segment_fault
global general_protection_fault
global page_fault

extern kprintf
extern write_panic_msg
extern hex2str
extern panic
extern vmm_map_page
extern pml4

%macro e_panic 1
    cli
    call write_panic_msg

    mov rdi, panic_msg
    mov rsi, %1
    call kprintf

    call panic
%endmacro

divide_error:
    e_panic 0x0

debug_exception:
    e_panic 0x1

overflow:
    e_panic 0x4

bound_range_exceeded:
    e_panic 0x5

invalid_opcode:
    e_panic 0x6

no_mathcoprocessor:
    e_panic 0x7

double_fault:
    e_panic 0x8

invalid_tss:
    e_panic 0xA

segment_not_present:
    e_panic 0xB

stack_segment_fault:
    e_panic 0xC

general_protection_fault:
    e_panic 0xD

page_fault:
    e_panic 0xE

panic_msg: db "VECTOR FIRED: %x", 0xA, 0
