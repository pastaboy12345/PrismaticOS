section .text

; Macro for creating interrupt stubs
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push qword 0      ; Push error code (0 for exceptions without error code)
    push qword %1     ; Push interrupt number
    jmp isr_common
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    push qword %1     ; Push interrupt number (error code already pushed by CPU)
    jmp isr_common
%endmacro

%macro IRQ 2
global irq%1
irq%1:
    push qword 0      ; Push error code
    push qword %2     ; Push interrupt number
    jmp irq_common
%endmacro

; Exception handlers (without error codes)
ISR_NOERRCODE 0      ; Divide by zero
ISR_NOERRCODE 1      ; Debug
ISR_NOERRCODE 2      ; Non-maskable interrupt
ISR_NOERRCODE 3      ; Breakpoint
ISR_NOERRCODE 4      ; Overflow
ISR_NOERRCODE 5      ; Bound range exceeded
ISR_NOERRCODE 6      ; Invalid opcode
ISR_NOERRCODE 7      ; Device not available
ISR_ERRCODE   8      ; Double fault
ISR_NOERRCODE 9      ; Reserved
ISR_ERRCODE   10     ; Invalid TSS
ISR_ERRCODE   11     ; Segment not present
ISR_ERRCODE   12     ; Stack-segment fault
ISR_ERRCODE   13     ; General protection fault
ISR_ERRCODE   14     ; Page fault
ISR_NOERRCODE 15     ; Reserved
ISR_NOERRCODE 16     ; x87 FPU error
ISR_ERRCODE   17     ; Alignment check
ISR_NOERRCODE 18     ; Machine check
ISR_NOERRCODE 19     ; SIMD FP exception

; IRQ handlers
IRQ 0, 32     ; Timer
IRQ 1, 33     ; Keyboard
IRQ 2, 34     ; Cascade
IRQ 3, 35     ; COM2
IRQ 4, 36     ; COM1
IRQ 5, 37     ; LPT2
IRQ 6, 38     ; Floppy
IRQ 7, 39     ; LPT1
IRQ 8, 40     ; RTC
IRQ 9, 41     ; Free
IRQ 10, 42    ; Free
IRQ 11, 43    ; Free
IRQ 12, 44    ; Mouse
IRQ 13, 45    ; FPU
IRQ 14, 46    ; ATA primary
IRQ 15, 47    ; ATA secondary

; System call handler
global isr128
isr128:
    push qword 0      ; Push error code
    push qword 128    ; Push interrupt number
    jmp syscall_common

; Common interrupt handler for exceptions
isr_common:
    ; Save all registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ; Save data segments
    mov ax, ds
    push rax
    
    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Call exception handler
    mov rdi, rsp      ; Pass CPU state pointer
    extern exception_handler
    call exception_handler
    
    ; Restore data segments
    pop rax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Restore all registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    
    ; Clean up error code and interrupt number
    add rsp, 16
    
    ; Return from interrupt
    iretq

; Common interrupt handler for IRQs
irq_common:
    ; Save all registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ; Save data segments
    mov ax, ds
    push rax
    
    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Call IRQ handler
    mov rdi, rsp      ; Pass CPU state pointer
    extern irq_handler
    call irq_handler
    
    ; Restore data segments
    pop rax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Restore all registers
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    
    ; Clean up error code and interrupt number
    add rsp, 16
    
    ; Return from interrupt
    iretq

; Common interrupt handler for system calls
syscall_common:
    ; Save all registers
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    
    ; Save data segments
    mov ax, ds
    push rax
    
    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Call syscall handler
    mov rdi, rsp      ; Pass CPU state pointer
    extern syscall_handler
    call syscall_handler
    
    ; Restore data segments
    pop rax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    ; Restore all registers except RAX (return value)
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    
    ; RAX is already set by syscall handler
    
    ; Clean up error code and interrupt number
    add rsp, 16
    
    ; Return from interrupt
    iretq
