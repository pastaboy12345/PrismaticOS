section .text
global _start

_start:
    ; Set up stack
    mov rsp, stack_top
    
    ; Clear BSS
    mov edi, bss_start
    mov ecx, bss_end - bss_start
    xor eax, eax
    rep stosb
    
    ; Call kernel main
    call kernel_main
    
.halt:
    cli
.halt_loop:
    hlt
    jmp .halt_loop

section .bss
    resb 8192  ; 8KB stack space
stack_top:

section .rodata
bss_start:
    dq bss_start
bss_end:
    dq bss_end
