[BITS 32]
[SECTION .text]

; External symbol
extern kernel_main

; Multiboot header
ALIGN 4
multiboot_header:
    dd 0x1BADB002              ; Magic number
    dd 0x00000003              ; Flags
    dd -(0x1BADB002 + 0x00000003) ; Checksum
    dd multiboot_header         ; Header address
    dd multiboot_header_end      ; Load end address
    dd 0                        ; BSS end address
    dd entry_point              ; Entry point
    dd 0                        ; Video mode type
    dd 0                        ; Width
    dd 0                        ; Height
    dd 0                        ; Depth

multiboot_header_end:
    dd 0

; Entry point
entry_point:
    ; Set up stack
    mov esp, stack_top
    
    ; Clear BSS
    mov edi, bss_start
    mov ecx, bss_end
    sub ecx, edi
    xor eax, eax
    cld
    rep stosb
    
    ; Call kernel main
    call kernel_main
    
    ; Halt if kernel returns
halt_loop:
    hlt
    jmp halt_loop

; Stack
SECTION .bss
ALIGN 4
stack_bottom:
    resb 0x10000  ; 64KB stack
stack_top:

; BSS section
bss_start:
bss_end:
