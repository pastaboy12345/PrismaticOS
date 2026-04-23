// Minimal working kernel for boot testing
#include "minimal_kernel.h"

void kernel_main(void) {
    // Simple kernel that just prints and halts
    const char *msg = "Modern OS Kernel v1.0 - Full kernel booted successfully!\n";
    
    // Write message to console using direct VGA memory access
    volatile char *vga = (volatile char*)0xB8000;
    for (int i = 0; msg[i]; i++) {
        vga[i * 2] = msg[i];
        vga[i * 2 + 1] = 0x0F; // White on black
    }
    
    // Halt
    while (1) {
        __asm__ volatile("hlt");
    }
}

void panic(const char *message) {
    const char *panic_msg = "KERNEL PANIC: ";
    volatile char *vga = (volatile char*)0xB8000;
    
    // Write panic message
    for (int i = 0; panic_msg[i]; i++) {
        vga[i * 2] = panic_msg[i];
        vga[i * 2 + 1] = 0x04; // Red on black
    }
    
    // Write panic message
    for (int i = 0; message[i]; i++) {
        vga[(i + 14) * 2] = message[i];
        vga[(i + 14) * 2 + 1] = 0x04; // Red on black
    }
    
    // Halt
    while (1) {
        __asm__ volatile("hlt");
    }
}
