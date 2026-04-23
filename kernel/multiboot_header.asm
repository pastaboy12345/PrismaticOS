[BITS 32]
[SECTION .text]

; Multiboot header
ALIGN 4
MULTIBOOT_HEADER:
    dd 0x1BADB002              ; Magic number
    dd MULTIBOOT_HEADER_FLAGS    ; Flags
    dd -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS) ; Checksum
    dd MULTIBOOT_HEADER_ADDR      ; Header address
    dd MULTIBOOT_HEADER_ADDR + MULTIBOOT_HEADER_SIZE ; Load end address
    dd 0                        ; BSS end address
    dd ENTRY                    ; Entry point
    dd 0                        ; Video mode type
    dd 640                      ; Width
    dd 480                      ; Height
    dd 32                       ; Depth

MULTIBOOT_HEADER_MAGIC   equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS   equ 0x00000003
MULTIBOOT_HEADER_ADDR    equ MULTIBOOT_HEADER
MULTIBOOT_HEADER_SIZE    equ $ - MULTIBOOT_HEADER_ADDR
ENTRY equ _start

; Jump to 64-bit code
jmp switch_to_long_mode

[BITS 64]
switch_to_long_mode:
    ; Set up basic 64-bit environment
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Enable long mode
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    
    ; Set up page tables and enable long mode
    ; For simplicity, we'll just jump to the kernel
    jmp kernel_main
