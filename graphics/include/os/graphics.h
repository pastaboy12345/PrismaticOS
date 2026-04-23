#ifndef OS_GRAPHICS_H
#define OS_GRAPHICS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Forward declarations
typedef struct os_surface os_surface_t;
typedef struct os_window os_window_t;
typedef struct os_paint_ctx os_paint_ctx_t;
typedef struct os_texture os_texture_t;
typedef struct os_font os_font_t;
typedef struct os_color os_color_t;
typedef struct os_rect os_rect_t;
typedef struct os_point os_point_t;
typedef struct os_size os_size_t;

// Color structure
struct os_color {
    float r, g, b, a;
};

// Rectangle structure
struct os_rect {
    float x, y, width, height;
};

// Point structure
struct os_point {
    float x, y;
};

// Size structure
struct os_size {
    float width, height;
};

// Window creation flags
#define OS_WINDOW_FLAG_RESIZABLE    0x01
#define OS_WINDOW_FLAG_BORDERLESS   0x02
#define OS_WINDOW_FLAG_ALWAYS_ON_TOP 0x04
#define OS_WINDOW_FLAG_FULLSCREEN    0x08

// Window management functions
os_window_t *os_window_create(const char *title, uint32_t width, uint32_t height, uint32_t flags);
void os_window_destroy(os_window_t *window);
void os_window_show(os_window_t *window);
void os_window_hide(os_window_t *window);
void os_window_set_title(os_window_t *window, const char *title);
void os_window_set_size(os_window_t *window, uint32_t width, uint32_t height);
void os_window_set_position(os_window_t *window, int32_t x, int32_t y);
void os_window_get_size(os_window_t *window, uint32_t *width, uint32_t *height);
void os_window_get_position(os_window_t *window, int32_t *x, int32_t *y);
bool os_window_is_visible(os_window_t *window);
bool os_window_has_focus(os_window_t *window);

// Surface management
os_surface_t *os_surface_create(uint32_t width, uint32_t height);
void os_surface_destroy(os_surface_t *surface);
void os_surface_resize(os_surface_t *surface, uint32_t width, uint32_t height);
void os_surface_get_size(os_surface_t *surface, uint32_t *width, uint32_t *height);
os_texture_t *os_surface_get_texture(os_surface_t *surface);

// Window-surface binding
void os_window_bind_surface(os_window_t *window, os_surface_t *surface);
os_surface_t *os_window_get_surface(os_window_t *window);

// Frame management
void os_surface_begin_frame(os_surface_t *surface);
void os_surface_end_frame(os_surface_t *surface);
void os_surface_present(os_surface_t *surface);
bool os_surface_needs_redraw(os_surface_t *surface);

// Drawing context
os_paint_ctx_t *os_paint_begin(os_surface_t *surface);
void os_paint_end(os_paint_ctx_t *ctx);

// Drawing primitives
void os_paint_clear(os_paint_ctx_t *ctx, const os_color_t *color);
void os_paint_fill_rect(os_paint_ctx_t *ctx, const os_rect_t *rect, const os_color_t *color);
void os_paint_stroke_rect(os_paint_ctx_t *ctx, const os_rect_t *rect, const os_color_t *color, float thickness);
void os_paint_fill_circle(os_paint_ctx_t *ctx, const os_point_t *center, float radius, const os_color_t *color);
void os_paint_stroke_circle(os_paint_ctx_t *ctx, const os_point_t *center, float radius, const os_color_t *color, float thickness);
void os_paint_fill_ellipse(os_paint_ctx_t *ctx, const os_rect_t *bounds, const os_color_t *color);
void os_paint_stroke_ellipse(os_paint_ctx_t *ctx, const os_rect_t *bounds, const os_color_t *color, float thickness);
void os_paint_draw_line(os_paint_ctx_t *ctx, const os_point_t *p1, const os_point_t *p2, const os_color_t *color, float thickness);
void os_paint_draw_lines(os_paint_ctx_t *ctx, const os_point_t *points, size_t count, const os_color_t *color, float thickness);
void os_paint_draw_polygon(os_paint_ctx_t *ctx, const os_point_t *points, size_t count, const os_color_t *color, float thickness);
void os_paint_fill_polygon(os_paint_ctx_t *ctx, const os_point_t *points, size_t count, const os_color_t *color);

// Path operations
typedef struct os_path os_path_t;
os_path_t *os_path_create(void);
void os_path_destroy(os_path_t *path);
void os_path_move_to(os_path_t *path, const os_point_t *point);
void os_path_line_to(os_path_t *path, const os_point_t *point);
void os_path_arc_to(os_path_t *path, const os_point_t *point, float radius, float start_angle, float sweep_angle);
void os_path_bezier_to(os_path_t *path, const os_point_t *cp1, const os_point_t *cp2, const os_point_t *end);
void os_path_close(os_path_t *path);
void os_paint_stroke_path(os_paint_ctx_t *ctx, os_path_t *path, const os_color_t *color, float thickness);
void os_paint_fill_path(os_paint_ctx_t *ctx, os_path_t *path, const os_color_t *color);

// Gradients
typedef struct os_gradient os_gradient_t;
typedef enum {
    OS_GRADIENT_LINEAR,
    OS_GRADIENT_RADIAL
} os_gradient_type_t;

os_gradient_t *os_gradient_create_linear(const os_point_t *start, const os_point_t *end);
os_gradient_t *os_gradient_create_radial(const os_point_t *center, float radius);
void os_gradient_destroy(os_gradient_t *gradient);
void os_gradient_add_color_stop(os_gradient_t *gradient, float offset, const os_color_t *color);
void os_paint_fill_rect_with_gradient(os_paint_ctx_t *ctx, const os_rect_t *rect, os_gradient_t *gradient);

// Images and textures
os_texture_t *os_texture_create_from_data(const uint8_t *data, uint32_t width, uint32_t height, uint32_t channels);
os_texture_t *os_texture_create_from_file(const char *filename);
void os_texture_destroy(os_texture_t *texture);
void os_texture_get_size(os_texture_t *texture, uint32_t *width, uint32_t *height);
void os_paint_draw_image(os_paint_ctx_t *ctx, os_texture_t *texture, const os_rect_t *dest_rect);
void os_paint_draw_image_subset(os_paint_ctx_t *ctx, os_texture_t *texture, const os_rect_t *src_rect, const os_rect_t *dest_rect);
void os_paint_draw_image_scaled(os_paint_ctx_t *ctx, os_texture_t *texture, const os_rect_t *dest_rect, bool preserve_aspect_ratio);

// Text rendering
os_font_t *os_font_create_from_file(const char *filename, float size);
os_font_t *os_font_create_default(float size);
void os_font_destroy(os_font_t *font);
void os_font_get_metrics(os_font_t *font, float *ascent, float *descent, float *height);
void os_font_get_text_size(os_font_t *font, const char *text, os_size_t *size);
typedef enum {
    OS_TEXT_ALIGN_LEFT,
    OS_TEXT_ALIGN_CENTER,
    OS_TEXT_ALIGN_RIGHT
} os_text_align_h_t;

typedef enum {
    OS_TEXT_ALIGN_TOP,
    OS_TEXT_ALIGN_MIDDLE,
    OS_TEXT_ALIGN_BOTTOM
} os_text_align_v_t;

void os_paint_draw_text(os_paint_ctx_t *ctx, const char *text, os_font_t *font, const os_point_t *position, const os_color_t *color);
void os_paint_draw_text_aligned(os_paint_ctx_t *ctx, const char *text, os_font_t *font, const os_rect_t *bounds, const os_color_t *color, os_text_align_h_t h_align, os_text_align_v_t v_align);

// Transform operations
void os_paint_push_transform(os_paint_ctx_t *ctx, const float matrix[16]);
void os_paint_pop_transform(os_paint_ctx_t *ctx);
void os_paint_set_transform(os_paint_ctx_t *ctx, const float matrix[16]);
void os_paint_get_transform(os_paint_ctx_t *ctx, float matrix[16]);
void os_paint_translate(os_paint_ctx_t *ctx, float tx, float ty);
void os_paint_scale(os_paint_ctx_t *ctx, float sx, float sy);
void os_paint_rotate(os_paint_ctx_t *ctx, float angle);
void os_paint_rotate_around(os_paint_ctx_t *ctx, float angle, const os_point_t *center);

// Clipping operations
void os_paint_set_clip_rect(os_paint_ctx_t *ctx, const os_rect_t *clip);
void os_paint_set_clip_path(os_paint_ctx_t *ctx, os_path_t *path);
void os_paint_clear_clip(os_paint_ctx_t *ctx);
void os_paint_get_clip_rect(os_paint_ctx_t *ctx, os_rect_t *clip);

// Blend modes
typedef enum {
    OS_BLEND_MODE_NORMAL,
    OS_BLEND_MODE_MULTIPLY,
    OS_BLEND_MODE_SCREEN,
    OS_BLEND_MODE_OVERLAY,
    OS_BLEND_MODE_DARKEN,
    OS_BLEND_MODE_LIGHTEN,
    OS_BLEND_MODE_COLOR_DODGE,
    OS_BLEND_MODE_COLOR_BURN,
    OS_BLEND_MODE_HARD_LIGHT,
    OS_BLEND_MODE_SOFT_LIGHT,
    OS_BLEND_MODE_DIFFERENCE,
    OS_BLEND_MODE_EXCLUSION
} os_blend_mode_t;

void os_paint_set_blend_mode(os_paint_ctx_t *ctx, os_blend_mode_t mode);
os_blend_mode_t os_paint_get_blend_mode(os_paint_ctx_t *ctx);

// Antialiasing
void os_paint_set_antialias(os_paint_ctx_t *ctx, bool enabled);
bool os_paint_get_antialias(os_paint_ctx_t *ctx);

// Utility functions
os_color_t os_color_from_rgb(uint8_t r, uint8_t g, uint8_t b);
os_color_t os_color_from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
os_color_t os_color_from_floats(float r, float g, float b, float a);
void os_rect_inflate(os_rect_t *rect, float dx, float dy);
void os_rect_offset(os_rect_t *rect, float dx, float dy);
bool os_rect_contains_point(const os_rect_t *rect, const os_point_t *point);
bool os_rect_intersects(const os_rect_t *rect1, const os_rect_t *rect2);
float os_point_distance(const os_point_t *p1, const os_point_t *p2);
void os_point_normalize(os_point_t *point);

// Constants
extern const os_color_t OS_COLOR_BLACK;
extern const os_color_t OS_COLOR_WHITE;
extern const os_color_t OS_COLOR_RED;
extern const os_color_t OS_COLOR_GREEN;
extern const os_color_t OS_COLOR_BLUE;
extern const os_color_t OS_COLOR_YELLOW;
extern const os_color_t OS_COLOR_CYAN;
extern const os_color_t OS_COLOR_MAGENTA;
extern const os_color_t OS_COLOR_TRANSPARENT;

#endif // OS_GRAPHICS_H
