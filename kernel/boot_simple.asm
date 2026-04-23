[BITS 64]
[SECTION .text]

[GLOBAL _start]
[EXTERN kernel_main]

_start:
    ; Set up stack (simple stack at 2MB)
    mov rsp, 0x200000
    
    ; Call kernel main
    call kernel_main
    
    ; Infinite loop if kernel returns
    cli
    hlt
    jmp $-2
