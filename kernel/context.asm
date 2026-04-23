section .text

; Context switch function
; Arguments: RDI = from_thread, RSI = to_thread
global context_switch
context_switch:
    ; Save current thread context
    ; RDI contains pointer to from_thread
    
    ; Save general purpose registers
    mov [rdi + 8], r15     ; r15 offset 8
    mov [rdi + 16], r14    ; r14 offset 16
    mov [rdi + 24], r13    ; r13 offset 24
    mov [rdi + 32], r12    ; r12 offset 32
    mov [rdi + 40], r11    ; r11 offset 40
    mov [rdi + 48], r10    ; r10 offset 48
    mov [rdi + 56], r9     ; r9 offset 56
    mov [rdi + 64], r8     ; r8 offset 64
    mov [rdi + 72], rbp    ; rbp offset 72
    mov [rdi + 80], rdi    ; rdi offset 80 (save original RDI)
    mov [rdi + 88], rsi    ; rsi offset 88
    mov [rdi + 96], rdx    ; rdx offset 96
    mov [rdi + 104], rcx   ; rcx offset 104
    mov [rdi + 112], rbx   ; rbx offset 112
    mov [rdi + 120], rax   ; rax offset 120
    
    ; Save stack pointer
    mov [rdi], rsp         ; rsp offset 0
    
    ; Save instruction pointer (will be saved on stack by call)
    mov [rdi + 128], r9    ; r9 offset 128 (temporarily use r9)
    
    ; Load new thread context
    ; RSI contains pointer to to_thread
    
    ; Load stack pointer
    mov rsp, [rsi]
    
    ; Load general purpose registers
    mov r15, [rsi + 8]
    mov r14, [rsi + 16]
    mov r13, [rsi + 24]
    mov r12, [rsi + 32]
    mov r11, [rsi + 40]
    mov r10, [rsi + 48]
    mov r9, [rsi + 56]
    mov r8, [rsi + 64]
    mov rbp, [rsi + 72]
    mov rdi, [rsi + 80]
    mov rsi, [rsi + 88]
    mov rdx, [rsi + 96]
    mov rcx, [rsi + 104]
    mov rbx, [rsi + 112]
    mov rax, [rsi + 120]
    
    ; Load instruction pointer
    mov r9, [rsi + 128]
    
    ; Jump to new thread
    jmp r9

; Thread entry wrapper
; Arguments: RDI = start_routine, RSI = arg
global thread_entry_wrapper
thread_entry_wrapper:
    ; Save arguments
    push rdi
    push rsi
    
    ; Call start routine
    mov rdi, [rsp + 8]   ; arg
    call [rsp]           ; start_routine
    
    ; Call thread_exit with return value
    mov rdi, rax         ; return value
    extern thread_exit
    call thread_exit
    
    ; Should never reach here
    hlt
