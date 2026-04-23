[BITS 32]

section .multiboot
align 4
multiboot_header:
dd 0x1BADB002
dd 0x00000003
dd -(0x1BADB002 + 0x00000003)
dd multiboot_header
dd 0
dd 0
dd 0
dd 0

section .text
global start
extern kernel_main

start:
    mov esp, stack_top
    call kernel_main
.halt:
    hlt
    jmp .halt

section .bss
align 4
stack_bottom:
    resb 16384
stack_top:
