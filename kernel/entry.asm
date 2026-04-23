[BITS 64]
[SECTION .text]

[GLOBAL _start]
[EXTERN kernel_main]
[EXTERN stack_top]

_start:
    ; Set up stack
    mov rsp, stack_top
    
    ; Call kernel main
    call kernel_main
    
    ; Halt if kernel returns (shouldn't happen)
    cli
    hlt
    jmp $-2
