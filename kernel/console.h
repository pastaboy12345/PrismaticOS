#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>
#include <stddef.h>

// Console colors
#define CONSOLE_COLOR_BLACK         0
#define CONSOLE_COLOR_BLUE          1
#define CONSOLE_COLOR_GREEN         2
#define CONSOLE_COLOR_CYAN          3
#define CONSOLE_COLOR_RED           4
#define CONSOLE_COLOR_MAGENTA       5
#define CONSOLE_COLOR_BROWN         6
#define CONSOLE_COLOR_LIGHT_GREY    7
#define CONSOLE_COLOR_DARK_GREY     8
#define CONSOLE_COLOR_LIGHT_BLUE    9
#define CONSOLE_COLOR_LIGHT_GREEN   10
#define CONSOLE_COLOR_LIGHT_CYAN    11
#define CONSOLE_COLOR_LIGHT_RED     12
#define CONSOLE_COLOR_LIGHT_MAGENTA 13
#define CONSOLE_COLOR_LIGHT_BROWN   14
#define CONSOLE_COLOR_WHITE         15

// Console dimensions
#define CONSOLE_WIDTH  80
#define CONSOLE_HEIGHT 25

// Console functions
void console_init(void);
void console_clear(void);
void console_putc(char c);
void console_puts(const char *str);
void console_printf(const char *format, ...);
void console_set_color(uint8_t fg, uint8_t bg);
void console_set_cursor(uint8_t x, uint8_t y);
void console_get_cursor(uint8_t *x, uint8_t *y);
void console_scroll_up(void);

// Kernel print function
void printk(const char *format, ...);

#endif // CONSOLE_H
