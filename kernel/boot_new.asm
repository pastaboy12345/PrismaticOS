[BITS 32]

; Multiboot header
ALIGN 4
mboot:
    MULTIBOOT_PAGE_ALIGN    equ 1<<0
    MULTIBOOT_MEMORY_INFO    equ 1<<1
    MULTIBOOT_AOUT_KLUDGE    equ 1<<16
    MULTIBOOT_HEADER_MAGIC    equ 0x1BADB002
    MULTIBOOT_HEADER_FLAGS    equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_AOUT_KLUDGE
    MULTIBOOT_CHECKSUM    equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
    
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd MULTIBOOT_CHECKSUM
    
    ; aout kludge - these are ignored when loaded from ELF
    dd mboot
    dd 0
    dd 0
    dd 0
    
    ; Align the entry point
    ALIGN 4
    
start:
    ; Set up stack
    mov esp, stack_top
    
    ; Call kernel main
    extern kernel_main
    call kernel_main
    
    ; Infinite loop
loop:
    hlt
    jmp loop

; Stack
SECTION .bss
ALIGN 4
stack_bottom:
    resb 0x10000  ; 64KB stack
stack_top:
