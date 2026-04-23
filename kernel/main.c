#include "kernel.h"
#include "mm.h"
#include "process.h"
#include "fs.h"
#include "syscall.h"
#include "interrupts.h"
#include "desktop.h"

// Forward declarations
void console_init(void);
void printk(const char *format, ...);

// Kernel entry point
void kernel_main(void) {
    // Initialize subsystems
    mm_init();
    process_init();
    fs_init();
    interrupts_init();
    syscall_init();
    
    // Set up basic console output
    console_init();
    
    printk("Modern OS Kernel v1.0\n");
    printk("Memory: %d MB available\n", mm_get_available_memory() / (1024 * 1024));
    printk("CPU: x86_64\n");
    
    // Initialize desktop environment
    desktop_init();
    
    // Create initial desktop windows
    desktop_shell_window();
    desktop_browser_window();
    
    // Present the desktop
    extern void graphics_present(void);
    graphics_present();
    
    printk("Desktop environment started\n");
    
    // Start scheduler
    scheduler_start();
    
    // Should never reach here
    panic("Kernel main returned unexpectedly");
}

void panic(const char *message) {
    printk("KERNEL PANIC: %s\n", message);
    
    // Disable interrupts
    __asm__ volatile("cli");
    
    // Halt the system
    while (1) {
        __asm__ volatile("hlt");
    }
}
