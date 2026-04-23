// Simple kernel graphics implementation
#include "kernel.h"

// Simple framebuffer graphics (VGA mode 0x13 for simplicity)
static uint16_t *framebuffer = (uint16_t*)0xA0000;
static int screen_width = 320;
static int screen_height = 200;

void graphics_init(void) {
    printk("Initializing simple graphics subsystem...\n");
    
    // Set VGA mode 0x13 (320x200x256 colors)
    __asm__ volatile(
        "mov $0x13, %al\n"
        "mov $0x00, %ah\n"
        "int $0x10\n"
    );
    
    printk("Graphics initialized (320x200x256)\n");
}

void graphics_clear_screen(uint32_t color) {
    // Simple clear - fill entire framebuffer
    uint16_t fill_color = (uint16_t)(color & 0xFF);
    for (int i = 0; i < screen_width * screen_height; i++) {
        framebuffer[i] = fill_color;
    }
}

void graphics_draw_rect(int x, int y, int width, int height, uint32_t color) {
    uint16_t fill_color = (uint16_t)(color & 0xFF);
    
    // Clamp coordinates
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + width > screen_width) width = screen_width - x;
    if (y + height > screen_height) height = screen_height - y;
    
    // Draw rectangle
    for (int py = y; py < y + height; py++) {
        for (int px = x; px < x + width; px++) {
            if (px < screen_width && py < screen_height) {
                framebuffer[py * screen_width + px] = fill_color;
            }
        }
    }
}

void graphics_draw_text(int x, int y, const char *text, uint32_t color) {
    if (!text) return;
    
    uint16_t text_color = (uint16_t)(color & 0xFF);
    
    // Simple 8x8 font rendering (just draw characters as blocks for now)
    int char_width = 8;
    int char_height = 8;
    
    for (int i = 0; text[i] && x + i * char_width < screen_width; i++) {
        // Draw character as a simple block
        for (int py = y; py < y + char_height && py < screen_height; py++) {
            for (int px = x + i * char_width; px < x + (i + 1) * char_width && px < screen_width; px++) {
                framebuffer[py * screen_width + px] = text_color;
            }
        }
    }
}

void graphics_present(void) {
    // In VGA mode, changes are immediate, so no present needed
}
