// Built-in desktop environment
#include "kernel.h"
#include "console.h"
#include <string.h>

// External graphics and HTML functions
extern void graphics_init(void);
extern void html_init(void);
extern void js_init(void);
extern void graphics_clear_screen(uint32_t color);
extern void graphics_draw_rect(int x, int y, int width, int height, uint32_t color);
extern void graphics_draw_text(int x, int y, const char *text, uint32_t color);

// Memory functions
extern void *kmalloc(size_t size);
extern void kfree(void *ptr);

// Desktop colors
#define DESKTOP_BG     0x2E4057  // Dark blue
#define TASKBAR_BG     0x1E1E1E  // Dark gray
#define TASKBAR_HEIGHT 40
#define WINDOW_BG      0xFFFFFF  // White
#define WINDOW_BORDER  0x000000  // Black
#define TEXT_COLOR     0x000000  // Black

// Window structure
typedef struct desktop_window {
    int x, y, width, height;
    char title[64];
    int visible;
    struct desktop_window *next;
} desktop_window_t;

static desktop_window_t *windows = NULL;
static desktop_window_t *focused_window = NULL;

void desktop_init(void) {
    printk("Initializing desktop environment...\n");
    
    // Initialize graphics subsystem
    graphics_init();
    html_init();
    js_init();
    
    // Clear screen with desktop background
    graphics_clear_screen(DESKTOP_BG);
    
    // Draw taskbar
    graphics_draw_rect(0, 0, 1024, TASKBAR_HEIGHT, TASKBAR_BG);
    
    // Draw start button
    graphics_draw_rect(5, 5, 80, 30, 0x4CAF50);  // Green
    graphics_draw_text(25, 15, "Start", 0xFFFFFF);  // White text
    
    printk("Desktop initialized\n");
}

desktop_window_t *desktop_create_window(int x, int y, int width, int height, const char *title) {
    desktop_window_t *window = kmalloc(sizeof(desktop_window_t));
    if (!window) return NULL;
    
    window->x = x;
    window->y = y;
    window->width = width;
    window->height = height;
    strncpy(window->title, title, sizeof(window->title) - 1);
    window->title[sizeof(window->title) - 1] = '\0';
    window->visible = 1;
    window->next = windows;
    windows = window;
    
    return window;
}

void desktop_draw_window(desktop_window_t *window) {
    if (!window || !window->visible) return;
    
    // Draw window background
    graphics_draw_rect(window->x, window->y, window->width, window->height, WINDOW_BG);
    
    // Draw window border
    graphics_draw_rect(window->x, window->y, window->width, 2, WINDOW_BORDER);           // Top
    graphics_draw_rect(window->x, window->y + window->height - 2, window->width, 2, WINDOW_BORDER); // Bottom
    graphics_draw_rect(window->x, window->y, 2, window->height, WINDOW_BORDER);           // Left
    graphics_draw_rect(window->x + window->width - 2, window->y, 2, window->height, WINDOW_BORDER);  // Right
    
    // Draw title bar
    graphics_draw_rect(window->x, window->y, window->width, 25, 0x4285F4);  // Blue
    graphics_draw_text(window->x + 5, window->y + 8, window->title, 0xFFFFFF);  // White text
    
    // Draw close button
    graphics_draw_rect(window->x + window->width - 25, window->y + 5, 20, 15, 0xF44336);  // Red
    graphics_draw_text(window->x + window->width - 20, window->y + 8, "X", 0xFFFFFF);  // White text
}

void desktop_redraw(void) {
    // Clear and redraw desktop
    graphics_clear_screen(DESKTOP_BG);
    
    // Redraw taskbar
    graphics_draw_rect(0, 0, 1024, TASKBAR_HEIGHT, TASKBAR_BG);
    graphics_draw_rect(5, 5, 80, 30, 0x4CAF50);
    graphics_draw_text(25, 15, "Start", 0xFFFFFF);
    
    // Redraw all windows
    desktop_window_t *window = windows;
    while (window) {
        desktop_draw_window(window);
        window = window->next;
    }
}

void desktop_shell_window(void) {
    // Create a shell window
    desktop_window_t *shell_win = desktop_create_window(100, 100, 600, 400, "Shell");
    if (shell_win) {
        focused_window = shell_win;
        desktop_redraw();
        
        // Draw shell content area
        graphics_draw_rect(shell_win->x + 5, shell_win->y + 30, 
                         shell_win->width - 10, shell_win->height - 35, 0xF5F5F5);  // Light gray
        
        // Draw shell prompt
        graphics_draw_text(shell_win->x + 10, shell_win->y + 40, 
                          "Modern OS Shell v1.0", TEXT_COLOR);
        graphics_draw_text(shell_win->x + 10, shell_win->y + 60, 
                          "Type 'help' for available commands", TEXT_COLOR);
        graphics_draw_text(shell_win->x + 10, shell_win->y + 90, 
                          "$ ", TEXT_COLOR);
    }
}

void desktop_browser_window(void) {
    // Create a browser window to demonstrate HTML/JS rendering
    desktop_window_t *browser_win = desktop_create_window(150, 150, 700, 500, "Web Browser");
    if (browser_win) {
        desktop_redraw();
        
        // Draw browser content area
        graphics_draw_rect(browser_win->x + 5, browser_win->y + 30, 
                         browser_win->width - 10, browser_win->height - 35, 0xFFFFFF);
        
        // Draw URL bar
        graphics_draw_rect(browser_win->x + 5, browser_win->y + 30, 
                         browser_win->width - 10, 25, 0xF0F0F0);
        graphics_draw_text(browser_win->x + 10, browser_win->y + 37, 
                          "file:///desktop.html", TEXT_COLOR);
        
        // Draw sample HTML content
        graphics_draw_text(browser_win->x + 10, browser_win->y + 70, 
                          "Modern OS Browser", TEXT_COLOR);
        graphics_draw_text(browser_win->x + 10, browser_win->y + 90, 
                          "Rendering HTML with JavaScript support", TEXT_COLOR);
        graphics_draw_text(browser_win->x + 10, browser_win->y + 110, 
                          "Built with our custom HTML/CSS/JS engine", TEXT_COLOR);
    }
}
