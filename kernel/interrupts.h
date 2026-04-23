#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

// Interrupt descriptor table entry
struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed));

// IDT pointer structure
struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

// Exception numbers
#define EXCEPTION_DIVIDE_BY_ZERO      0
#define EXCEPTION_DEBUG              1
#define EXCEPTION_NMI                2
#define EXCEPTION_BREAKPOINT         3
#define EXCEPTION_OVERFLOW          4
#define EXCEPTION_BOUND_RANGE        5
#define EXCEPTION_INVALID_OPCODE     6
#define EXCEPTION_DEVICE_NOT_AVAIL   7
#define EXCEPTION_DOUBLE_FAULT       8
#define EXCEPTION_INVALID_TSS        10
#define EXCEPTION_SEGMENT_NOT_PRESENT 11
#define EXCEPTION_STACK_FAULT        12
#define EXCEPTION_GENERAL_PROTECTION 13
#define EXCEPTION_PAGE_FAULT        14
#define EXCEPTION_X87_FPU_ERROR     16
#define EXCEPTION_ALIGNMENT_CHECK   17
#define EXCEPTION_MACHINE_CHECK      18
#define EXCEPTION_SIMD_FP           19

// System call interrupt
#define SYSCALL_INTERRUPT           0x80

// IRQ lines
#define IRQ_TIMER                    0
#define IRQ_KEYBOARD                 1
#define IRQ_CASCADE                  2
#define IRQ_COM2                     3
#define IRQ_COM1                     4
#define IRQ_LPT2                     5
#define IRQ_FLOPPY                   6
#define IRQ_LPT1                     7
#define IRQ_RTC                      8
#define IRQ_FREE1                    9
#define IRQ_FREE2                    10
#define IRQ_MOUSE                    12
#define IRQ_FPU                      13
#define IRQ_ATA_PRIMARY              14
#define IRQ_ATA_SECONDARY            15

// PIC ports
#define PIC1_COMMAND    0x20
#define PIC1_DATA       0x21
#define PIC2_COMMAND    0xA0
#define PIC2_DATA       0xA1

// PIC commands
#define PIC_EOI         0x20
#define PIC_INIT        0x11
#define PIC_ICW4_8086   0x01

// APIC registers (memory mapped)
#define APIC_BASE       0xFEE00000
#define APIC_ID         (APIC_BASE + 0x20)
#define APIC_EOI        (APIC_BASE + 0xB0)
#define APIC_SVR        (APIC_BASE + 0xF0)
#define APIC_LVT_TIMER  (APIC_BASE + 0x320)
#define APIC_TIMER_INIT (APIC_BASE + 0x380)
#define APIC_TIMER_CUR  (APIC_BASE + 0x390)

// CPU registers structure for interrupt handling
struct cpu_state {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, eflags, rsp, ss;
} __attribute__((packed));

// Function declarations
void interrupts_init(void);
void idt_set_gate(uint8_t num, uint64_t handler, uint16_t selector, uint8_t type_attr);
void pic_remap(uint8_t offset1, uint8_t offset2);
void pic_send_eoi(uint8_t irq);

// Interrupt handlers
void exception_handler(struct cpu_state *cpu);
void irq_handler(struct cpu_state *cpu);
void syscall_handler(struct cpu_state *cpu);

// Timer functions
void timer_init(uint32_t frequency);
void timer_set_frequency(uint32_t frequency);
void timer_tick(void);

// Keyboard functions
void keyboard_init(void);
uint8_t keyboard_read_scancode(void);
char keyboard_scancode_to_ascii(uint8_t scancode);

// Common interrupt stubs
extern void isr0(void);   // Divide by zero
extern void isr1(void);   // Debug
extern void isr2(void);   // Non-maskable interrupt
extern void isr3(void);   // Breakpoint
extern void isr4(void);   // Overflow
extern void isr5(void);   // Bound range exceeded
extern void isr6(void);   // Invalid opcode
extern void isr7(void);   // Device not available
extern void isr8(void);   // Double fault
extern void isr10(void);  // Invalid TSS
extern void isr11(void);  // Segment not present
extern void isr12(void);  // Stack-segment fault
extern void isr13(void);  // General protection fault
extern void isr14(void);  // Page fault
extern void isr16(void);  // x87 FPU error
extern void isr17(void);  // Alignment check
extern void isr18(void);  // Machine check
extern void isr19(void);  // SIMD FP exception

extern void irq0(void);   // Timer
extern void irq1(void);   // Keyboard
extern void irq2(void);   // Cascade
extern void irq3(void);   // COM2
extern void irq4(void);   // COM1
extern void irq5(void);   // LPT2
extern void irq6(void);   // Floppy
extern void irq7(void);   // LPT1
extern void irq8(void);   // RTC
extern void irq9(void);   // Free
extern void irq10(void);  // Free
extern void irq11(void);  // Free
extern void irq12(void);  // Mouse
extern void irq13(void);  // FPU
extern void irq14(void);  // ATA primary
extern void irq15(void);  // ATA secondary

extern void isr128(void); // System call

#endif // INTERRUPTS_H
