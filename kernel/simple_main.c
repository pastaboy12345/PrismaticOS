// Simple kernel main for testing QEMU boot
void kernel_main(void) {
    // Simple kernel that just prints and halts
    const char *msg = "Modern OS Kernel v1.0 - Booted successfully!\n";
    
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
