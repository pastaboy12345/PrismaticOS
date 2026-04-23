[BITS 32]
[SECTION .text]

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

entry_point:
    ; Set up basic environment
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Enable A20 gate (simplified)
    in al, 0x92
    or al, 2
    out 0x92, al
    
    ; Switch to protected mode (simplified)
    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax
    
    ; Jump to 64-bit code
    lgdt [gdt_pointer]
    jmp 0x08:long_mode_start

[BITS 64]
long_mode_start:
    ; Set up 64-bit segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Set up stack
    mov rsp, 0x200000
    
    ; Call kernel main
    extern kernel_main
    call kernel_main
    
    ; Halt if kernel returns
.halt:
    cli
    hlt
    jmp .halt

; GDT
gdt:
    dq 0x00000000000000000  ; Null descriptor
    dq 0x00CF9A000000FFFF  ; Code descriptor (64-bit)
    dq 0x00CF92000000FFFF  ; Data descriptor (64-bit)

gdt_pointer:
    dw gdt_pointer - gdt - 1
    dq gdt
