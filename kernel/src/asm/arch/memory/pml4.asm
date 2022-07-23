;;
;; Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
;; See LICENSE file.

bits 64

global load_pml4
global _invlpg

load_pml4:  
    mov cr3, rdi
    retq

_invlpg:
    invlpg [rdi]
    retq
