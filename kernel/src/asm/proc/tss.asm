;; Copyright (c) 2022 N3xusProject, Ian Marco Moffett, All Rights Reserved.
;; See LICENSE file.
bits 64

global load_tss

load_tss:
    str ax
    mov ax, 0x48
    ltr ax
    retq
