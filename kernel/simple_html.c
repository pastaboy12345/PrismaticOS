// Simple HTML implementation for kernel
#include "kernel.h"

void html_init(void) {
    printk("Initializing simple HTML subsystem...\n");
    printk("HTML subsystem initialized\n");
}

void html_render_page(const char *html_content, const char *css_content) {
    if (!html_content) return;
    
    printk("Rendering HTML page...\n");
    
    // Simple HTML rendering - just extract and display text content
    int in_tag = 0;
    int x = 10, y = 40;
    
    for (int i = 0; html_content[i]; i++) {
        if (html_content[i] == '<') {
            in_tag = 1;
            continue;
        }
        if (html_content[i] == '>') {
            in_tag = 0;
            if (html_content[i-1] == 'p' || html_content[i-1] == 'P') {
                y += 20;  // New paragraph
                x = 10;
            }
            continue;
        }
        
        if (!in_tag && html_content[i] != '\n' && html_content[i] != '\r') {
            // Draw character
            char str[2] = {html_content[i], 0};
            extern void graphics_draw_text(int x, int y, const char *text, uint32_t color);
            graphics_draw_text(x, y, str, 0x000000);
            x += 8;
            
            // Simple word wrap
            if (x > 300) {
                x = 10;
                y += 12;
            }
        }
    }
}
