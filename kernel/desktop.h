#ifndef DESKTOP_H
#define DESKTOP_H

// Desktop initialization and management
void desktop_init(void);
void desktop_redraw(void);
void desktop_shell_window(void);
void desktop_browser_window(void);

// Window management
typedef struct desktop_window desktop_window_t;
desktop_window_t *desktop_create_window(int x, int y, int width, int height, const char *title);

#endif // DESKTOP_H
