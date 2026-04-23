#include "console.h"
#include <stdarg.h>
#include <string.h>

// VGA text mode buffer
static uint16_t *vga_buffer = (uint16_t*)0xB8000;

// Console state
static uint8_t console_x = 0;
static uint8_t console_y = 0;
static uint8_t console_color = 0x07;  // Light grey on black

// VGA color attribute
static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg) {
    return fg | (bg << 4);
}

// VGA entry character
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}

// Initialize console
void console_init(void) {
    console_clear();
    console_set_color(CONSOLE_COLOR_LIGHT_GREY, CONSOLE_COLOR_BLACK);
}

// Clear console
void console_clear(void) {
    for (int y = 0; y < CONSOLE_HEIGHT; y++) {
        for (int x = 0; x < CONSOLE_WIDTH; x++) {
            const size_t index = y * CONSOLE_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', console_color);
        }
    }
    
    console_x = 0;
    console_y = 0;
}

// Put character
void console_putc(char c) {
    if (c == '\n') {
        console_x = 0;
        console_y++;
    } else if (c == '\r') {
        console_x = 0;
    } else if (c == '\t') {
        console_x = (console_x + 8) & ~7;
    } else if (c >= ' ') {
        const size_t index = console_y * CONSOLE_WIDTH + console_x;
        vga_buffer[index] = vga_entry((unsigned char)c, console_color);
        console_x++;
    }
    
    if (console_x >= CONSOLE_WIDTH) {
        console_x = 0;
        console_y++;
    }
    
    if (console_y >= CONSOLE_HEIGHT) {
        console_scroll_up();
        console_y = CONSOLE_HEIGHT - 1;
    }
}

// Put string
void console_puts(const char *str) {
    while (*str) {
        console_putc(*str++);
    }
}

// Set color
void console_set_color(uint8_t fg, uint8_t bg) {
    console_color = vga_entry_color(fg, bg);
}

// Set cursor position
void console_set_cursor(uint8_t x, uint8_t y) {
    if (x < CONSOLE_WIDTH && y < CONSOLE_HEIGHT) {
        console_x = x;
        console_y = y;
    }
}

// Get cursor position
void console_get_cursor(uint8_t *x, uint8_t *y) {
    if (x) *x = console_x;
    if (y) *y = console_y;
}

// Scroll up one line
void console_scroll_up(void) {
    // Move all lines up
    for (int y = 0; y < CONSOLE_HEIGHT - 1; y++) {
        for (int x = 0; x < CONSOLE_WIDTH; x++) {
            const size_t src_index = (y + 1) * CONSOLE_WIDTH + x;
            const size_t dst_index = y * CONSOLE_WIDTH + x;
            vga_buffer[dst_index] = vga_buffer[src_index];
        }
    }
    
    // Clear last line
    for (int x = 0; x < CONSOLE_WIDTH; x++) {
        const size_t index = (CONSOLE_HEIGHT - 1) * CONSOLE_WIDTH + x;
        vga_buffer[index] = vga_entry(' ', console_color);
    }
}

// Simple printf implementation
static void console_print_int(int value, int base) {
    if (value == 0) {
        console_putc('0');
        return;
    }
    
    char buffer[32];
    int index = 0;
    int negative = 0;
    
    if (value < 0 && base == 10) {
        negative = 1;
        value = -value;
    }
    
    while (value > 0) {
        int digit = value % base;
        buffer[index++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
        value /= base;
    }
    
    if (negative) {
        console_putc('-');
    }
    
    while (index > 0) {
        console_putc(buffer[--index]);
    }
}

static void console_print_uint(uint32_t value, int base) {
    if (value == 0) {
        console_putc('0');
        return;
    }
    
    char buffer[32];
    int index = 0;
    
    while (value > 0) {
        uint32_t digit = value % base;
        value /= base;
        buffer[index++] = "0123456789abcdef"[digit];
    }
    
    while (index > 0) {
        console_putc(buffer[--index]);
    }
}

void console_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    while (*format) {
        if (*format == '%') {
            format++;
            
            switch (*format) {
                case 'c':
                    console_putc(va_arg(args, int));
                    break;
                case 's':
                    console_puts(va_arg(args, const char*));
                    break;
                case 'd':
                    console_print_int(va_arg(args, int), 10);
                    break;
                case 'u':
                    console_print_uint(va_arg(args, unsigned int), 10);
                    break;
                case 'x':
                    console_print_uint(va_arg(args, unsigned int), 16);
                    break;
                case 'p':
                    console_puts("0x");
                    console_print_uint((uint32_t)va_arg(args, void*), 16);
                    break;
                case 'l':
                    format++;
                    if (*format == 'd') {
                        console_print_int(va_arg(args, long), 10);
                    } else if (*format == 'u') {
                        console_print_uint(va_arg(args, unsigned long), 10);
                    } else if (*format == 'x') {
                        console_print_uint(va_arg(args, unsigned long), 16);
                    }
                    break;
                case '%':
                    console_putc('%');
                    break;
                default:
                    console_putc('%');
                    console_putc(*format);
                    break;
            }
        } else {
            console_putc(*format);
        }
        
        format++;
    }
    
    va_end(args);
}

// Kernel print function (alias for console_printf)
void printk(const char *format, ...) {
    va_list args;
    va_start(args, format);
    console_printf(format, args);
    va_end(args);
}
