#include "interrupts.h"

// Forward declarations
void printk(const char *format, ...);
void timer_init(uint32_t frequency);
void keyboard_init(void);
#include "kernel.h"
#include "process.h"
#include <string.h>

// IDT table
static struct idt_entry idt[256];
static struct idt_ptr idt_ptr;

// Exception messages
static const char *exception_messages[32] = {
    "Division by zero",
    "Debug",
    "Non-maskable interrupt",
    "Breakpoint",
    "Overflow",
    "Bound range exceeded",
    "Invalid opcode",
    "Device not available",
    "Double fault",
    "Reserved",
    "Invalid TSS",
    "Segment not present",
    "Stack-segment fault",
    "General protection fault",
    "Page fault",
    "Reserved",
    "x87 FPU error",
    "Alignment check",
    "Machine check",
    "SIMD FP exception",
    "Virtualization exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Security exception",
    "Reserved"
};

// Set IDT gate
void idt_set_gate(uint8_t num, uint64_t handler, uint16_t selector, uint8_t type_attr) {
    idt[num].offset_low = handler & 0xFFFF;
    idt[num].offset_middle = (handler >> 16) & 0xFFFF;
    idt[num].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[num].selector = selector;
    idt[num].ist = 0;
    idt[num].type_attr = type_attr;
    idt[num].zero = 0;
}

// Remap PIC interrupts
void pic_remap(uint8_t offset1, uint8_t offset2) {
    // Save masks
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);

    // Start initialization sequence
    outb(PIC1_COMMAND, PIC_INIT);
    outb(PIC2_COMMAND, PIC_INIT);
    
    // Set vector offsets
    outb(PIC1_DATA, offset1);
    outb(PIC2_DATA, offset2);
    
    // Configure cascade
    outb(PIC1_DATA, 4);
    outb(PIC2_DATA, 2);
    
    // Set 8086 mode
    outb(PIC1_DATA, PIC_ICW4_8086);
    outb(PIC2_DATA, PIC_ICW4_8086);
    
    // Restore masks
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

// Send End of Interrupt signal
void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

// Exception handler
void exception_handler(struct cpu_state *cpu) {
    uint32_t int_no = cpu->int_no;
    
    if (int_no < 32) {
        printk("Exception %d: %s\n", int_no, exception_messages[int_no]);
        printk("RIP: 0x%016x\n", cpu->rip);
        printk("RSP: 0x%016x\n", cpu->rsp);
        printk("RAX: 0x%016x\n", cpu->rax);
        printk("RBX: 0x%016x\n", cpu->rbx);
        printk("RCX: 0x%016x\n", cpu->rcx);
        printk("RDX: 0x%016x\n", cpu->rdx);
        printk("Error code: 0x%016x\n", cpu->err_code);
        
        panic("Unhandled exception");
    } else {
        printk("Unknown exception: %d\n", int_no);
        panic("Unknown exception");
    }
}

// IRQ handler
void irq_handler(struct cpu_state *cpu) {
    uint32_t int_no = cpu->int_no - 32;  // Convert to IRQ number
    
    switch (int_no) {
        case IRQ_TIMER:
            timer_tick();
            break;
        case IRQ_KEYBOARD:
            keyboard_handler();
            break;
        default:
            printk("Unhandled IRQ: %d\n", int_no);
            break;
    }
    
    pic_send_eoi(int_no);
}

// System call handler
void syscall_handler(struct cpu_state *cpu) {
    // System call number is in RAX
    uint64_t syscall_num = cpu->rax;
    
    // Arguments are in RDI, RSI, RDX, R10, R8, R9
    uint64_t arg1 = cpu->rdi;
    uint64_t arg2 = cpu->rsi;
    uint64_t arg3 = cpu->rdx;
    uint64_t arg4 = cpu->r10;
    uint64_t arg5 = cpu->r8;
    uint64_t arg6 = cpu->r9;
    
    // Call kernel syscall handler
    extern void kernel_syscall_handler(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);
    kernel_syscall_handler(syscall_num, arg1, arg2, arg3, arg4, arg5, arg6);
    
    // Return value is already set in RAX by the syscall handler
}

// Timer initialization
void timer_init(uint32_t frequency) {
    timer_set_frequency(frequency);
}

void timer_set_frequency(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;
    
    outb(0x43, 0x36);  // Command byte
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

void timer_tick(void) {
    static uint32_t tick = 0;
    tick++;
    
    // Call scheduler tick
    scheduler_tick();
}

// Keyboard handler
void keyboard_handler(void) {
    uint8_t scancode = inb(0x60);
    
    if (scancode & 0x80) {
        // Key released
    } else {
        // Key pressed
        char c = keyboard_scancode_to_ascii(scancode);
        if (c != 0) {
            // Handle character input
            // TODO: add to input buffer
        }
    }
}

uint8_t keyboard_read_scancode(void) {
    return inb(0x60);
}

char keyboard_scancode_to_ascii(uint8_t scancode) {
    static const char scancode_table[128] = {
        0, 0, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
        0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
        '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    
    if (scancode < 128) {
        return scancode_table[scancode];
    }
    
    return 0;
}

// Initialize interrupt system
void interrupts_init(void) {
    // Clear IDT
    memset(&idt, 0, sizeof(idt));
    
    // Set up IDT pointer
    idt_ptr.limit = sizeof(idt) - 1;
    idt_ptr.base = (uint64_t)&idt;
    
    // Install exception handlers
    idt_set_gate(0, (uint64_t)isr0, 0x08, 0x8E);
    idt_set_gate(1, (uint64_t)isr1, 0x08, 0x8E);
    idt_set_gate(2, (uint64_t)isr2, 0x08, 0x8E);
    idt_set_gate(3, (uint64_t)isr3, 0x08, 0x8E);
    idt_set_gate(4, (uint64_t)isr4, 0x08, 0x8E);
    idt_set_gate(5, (uint64_t)isr5, 0x08, 0x8E);
    idt_set_gate(6, (uint64_t)isr6, 0x08, 0x8E);
    idt_set_gate(7, (uint64_t)isr7, 0x08, 0x8E);
    idt_set_gate(8, (uint64_t)isr8, 0x08, 0x8E);
    idt_set_gate(10, (uint64_t)isr10, 0x08, 0x8E);
    idt_set_gate(11, (uint64_t)isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint64_t)isr12, 0x08, 0x8E);
    idt_set_gate(13, (uint64_t)isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint64_t)isr14, 0x08, 0x8E);
    idt_set_gate(16, (uint64_t)isr16, 0x08, 0x8E);
    idt_set_gate(17, (uint64_t)isr17, 0x08, 0x8E);
    idt_set_gate(18, (uint64_t)isr18, 0x08, 0x8E);
    idt_set_gate(19, (uint64_t)isr19, 0x08, 0x8E);
    
    // Install IRQ handlers
    idt_set_gate(32, (uint64_t)irq0, 0x08, 0x8E);
    idt_set_gate(33, (uint64_t)irq1, 0x08, 0x8E);
    idt_set_gate(34, (uint64_t)irq2, 0x08, 0x8E);
    idt_set_gate(35, (uint64_t)irq3, 0x08, 0x8E);
    idt_set_gate(36, (uint64_t)irq4, 0x08, 0x8E);
    idt_set_gate(37, (uint64_t)irq5, 0x08, 0x8E);
    idt_set_gate(38, (uint64_t)irq6, 0x08, 0x8E);
    idt_set_gate(39, (uint64_t)irq7, 0x08, 0x8E);
    idt_set_gate(40, (uint64_t)irq8, 0x08, 0x8E);
    idt_set_gate(41, (uint64_t)irq9, 0x08, 0x8E);
    idt_set_gate(42, (uint64_t)irq10, 0x08, 0x8E);
    idt_set_gate(43, (uint64_t)irq11, 0x08, 0x8E);
    idt_set_gate(44, (uint64_t)irq12, 0x08, 0x8E);
    idt_set_gate(45, (uint64_t)irq13, 0x08, 0x8E);
    idt_set_gate(46, (uint64_t)irq14, 0x08, 0x8E);
    idt_set_gate(47, (uint64_t)irq15, 0x08, 0x8E);
    
    // Install system call handler
    idt_set_gate(128, (uint64_t)isr128, 0x08, 0x8E);
    
    // Load IDT
    __asm__ volatile("lidt %0" : : "m" (idt_ptr));
    
    // Remap PIC
    pic_remap(32, 40);
    
    // Enable interrupts
    __asm__ volatile("sti");
    
    // Initialize timer
    timer_init(100);  // 100 Hz
    
    // Initialize keyboard
    keyboard_init();
}

// I/O port functions
static inline void outb(uint16_t port __attribute__((unused)), uint8_t value __attribute__((unused))) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port __attribute__((unused))) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
