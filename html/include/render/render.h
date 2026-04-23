#ifndef RENDER_RENDER_H
#define RENDER_RENDER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <dom/dom.h>
#include <css/css.h>
#include <layout/layout.h>
#include <os/graphics.h>

// Render layer types
typedef enum {
    RENDER_LAYER_BACKGROUND,
    RENDER_LAYER_BORDER,
    RENDER_LAYER_CONTENT,
    RENDER_LAYER_OUTLINE,
    RENDER_LAYER_OVERFLOW,
    RENDER_LAYER_SCROLLBAR,
    RENDER_LAYER_SELECTION,
    RENDER_LAYER_CURSOR,
    RENDER_LAYER_CUSTOM
} render_layer_type_t;

// Display list operations
typedef enum {
    RENDER_OP_CLEAR,
    RENDER_OP_FILL_RECT,
    RENDER_OP_STROKE_RECT,
    RENDER_OP_FILL_PATH,
    RENDER_OP_STROKE_PATH,
    RENDER_OP_DRAW_IMAGE,
    RENDER_OP_DRAW_TEXT,
    RENDER_OP_PUSH_CLIP,
    RENDER_OP_POP_CLIP,
    RENDER_OP_PUSH_TRANSFORM,
    RENDER_OP_POP_TRANSFORM,
    RENDER_OP_SET_BLEND_MODE,
    RENDER_OP_SET_ANTIALIAS,
    RENDER_OP_DRAW_LINE,
    RENDER_OP_DRAW_LINES,
    RENDER_OP_DRAW_ELLIPSE,
    RENDER_OP_DRAW_GRADIENT
} render_op_type_t;

// Display list command
typedef struct render_cmd {
    render_op_type_t type;
    
    // Command data
    union {
        struct {
            os_color_t color;
            os_rect_t rect;
        } fill_rect;
        struct {
            os_color_t color;
            os_rect_t rect;
            float thickness;
        } stroke_rect;
        struct {
            os_path_t *path;
            os_color_t color;
        } fill_path;
        struct {
            os_path_t *path;
            os_color_t color;
            float thickness;
        } stroke_path;
        struct {
            os_texture_t *texture;
            os_rect_t src_rect;
            os_rect_t dst_rect;
        } draw_image;
        struct {
            char *text;
            os_font_t *font;
            os_point_t position;
            os_color_t color;
        } draw_text;
        struct {
            os_rect_t rect;
        } push_clip;
        struct {
            float matrix[16];
        } push_transform;
        struct {
            os_blend_mode_t mode;
        } set_blend_mode;
        struct {
            os_point_t p1, p2;
            os_color_t color;
            float thickness;
        } draw_line;
        struct {
            os_point_t *points;
            size_t count;
            os_color_t color;
            float thickness;
        } draw_lines;
        struct {
            os_rect_t bounds;
            os_color_t color;
        } draw_ellipse;
        struct {
            os_rect_t rect;
            os_gradient_t *gradient;
        } draw_gradient;
    } data;
    
    struct render_cmd *next;
} render_cmd_t;

// Display list
typedef struct display_list {
    render_cmd_t *commands;
    render_cmd_t *last_cmd;
    size_t command_count;
    size_t capacity;
    uint32_t ref_count;
    
    // Bounds for optimization
    os_rect_t bounds;
    bool bounds_valid;
    
    // Layer information
    render_layer_type_t layer;
    int z_order;
    
    // Caching
    uint64_t hash;
    bool is_cached;
} display_list_t;

// Render target
typedef struct render_target {
    os_surface_t *surface;
    os_texture_t *texture;
    uint32_t width;
    uint32_t height;
    gpu_format_t format;
    
    // Viewport
    os_rect_t viewport;
    
    // Transform
    float transform[16];
    
    // Clipping
    os_rect_t clip_rect;
    bool has_clip;
    
    // Layers
    display_list_t **layers;
    size_t layer_count;
    
    // Damage tracking
    os_rect_t *damage_rects;
    size_t damage_count;
    
    // State
    bool needs_redraw;
    bool is_dirty;
} render_target_t;

// Renderer
typedef struct renderer {
    layout_context_t *layout_context;
    render_target_t *main_target;
    
    // Rendering options
    bool antialias;
    bool subpixel_text;
    bool hardware_acceleration;
    
    // Performance metrics
    struct {
        uint64_t render_time_ms;
        uint64_t paint_time_ms;
        size_t command_count;
        size_t draw_calls;
        size_t texture_uploads;
        size_t cache_hits;
        size_t cache_misses;
    } metrics;
    
    // Caches
    struct {
        display_list_t **display_lists;
        size_t count;
        size_t capacity;
    } display_list_cache;
    
    struct {
        os_texture_t **textures;
        char **keys;
        size_t count;
        size_t capacity;
    } texture_cache;
    
    // State
    bool is_rendering;
    uint32_t frame_count;
    uint64_t last_frame_time;
} renderer_t;

// Display list management
display_list_t *display_list_create(void);
void display_list_destroy(display_list_t *dl);
display_list_t *display_list_ref(display_list_t *dl);
void display_list_unref(display_list_t *dl);
void display_list_clear(display_list_t *dl);
void display_list_add_command(display_list_t *dl, render_cmd_t *cmd);
void display_list_append(display_list_t *dl, display_list_t *other);
os_rect_t display_list_get_bounds(display_list_t *dl);
void display_list_invalidate_bounds(display_list_t *dl);

// Display list building
void display_list_clear_rect(display_list_t *dl, const os_rect_t *rect, const os_color_t *color);
void display_list_fill_rect(display_list_t *dl, const os_rect_t *rect, const os_color_t *color);
void display_list_stroke_rect(display_list_t *dl, const os_rect_t *rect, const os_color_t *color, float thickness);
void display_list_fill_path(display_list_t *dl, os_path_t *path, const os_color_t *color);
void display_list_stroke_path(display_list_t *dl, os_path_t *path, const os_color_t *color, float thickness);
void display_list_draw_image(display_list_t *dl, os_texture_t *texture, const os_rect_t *src_rect, const os_rect_t *dst_rect);
void display_list_draw_text(display_list_t *dl, const char *text, os_font_t *font, const os_point_t *position, const os_color_t *color);
void display_list_push_clip(display_list_t *dl, const os_rect_t *rect);
void display_list_push_path_clip(display_list_t *dl, os_path_t *path);
void display_list_pop_clip(display_list_t *dl);
void display_list_push_transform(display_list_t *dl, const float matrix[16]);
void display_list_pop_transform(display_list_t *dl);
void display_list_set_blend_mode(display_list_t *dl, os_blend_mode_t mode);
void display_list_set_antialias(display_list_t *dl, bool enabled);
void display_list_draw_line(display_list_t *dl, const os_point_t *p1, const os_point_t *p2, const os_color_t *color, float thickness);
void display_list_draw_lines(display_list_t *dl, const os_point_t *points, size_t count, const os_color_t *color, float thickness);
void display_list_draw_ellipse(display_list_t *dl, const os_rect_t *bounds, const os_color_t *color);
void display_list_fill_ellipse(display_list_t *dl, const os_rect_t *bounds, const os_color_t *color);
void display_list_draw_gradient(display_list_t *dl, const os_rect_t *rect, os_gradient_t *gradient);

// Render target management
render_target_t *render_target_create(uint32_t width, uint32_t height);
void render_target_destroy(render_target_t *target);
void render_target_resize(render_target_t *target, uint32_t width, uint32_t height);
void render_target_set_viewport(render_target_t *target, const os_rect_t *viewport);
void render_target_set_transform(render_target_t *target, const float matrix[16]);
void render_target_set_clip(render_target_t *target, const os_rect_t *clip);
void render_target_clear_clip(render_target_t *target);
void render_target_add_layer(render_target_t *target, display_list_t *dl, render_layer_type_t layer, int z_order);
void render_target_clear_layers(render_target_t *target);
void render_target_invalidate(render_target_t *target, const os_rect_t *rect);
void render_target_invalidate_all(render_target_t *target);

// Renderer management
renderer_t *renderer_create(layout_context_t *layout_context);
void renderer_destroy(renderer_t *renderer);
void renderer_set_main_target(renderer_t *renderer, render_target_t *target);
void renderer_set_options(renderer_t *renderer, bool antialias, bool subpixel_text, bool hardware_acceleration);

// Rendering pipeline
void renderer_begin_frame(renderer_t *renderer);
void renderer_end_frame(renderer_t *renderer);
void renderer_render(renderer_t *renderer);
void renderer_render_to_target(renderer_t *renderer, render_target_t *target);

// Layout to display list conversion
display_list_t *renderer_render_layout_box(renderer_t *renderer, layout_box_t *box);
display_list_t *renderer_render_text_box(renderer_t *renderer, layout_box_t *box);
display_list_t *renderer_render_replaced_box(renderer_t *renderer, layout_box_t *box);
display_list_t *renderer_render_background(renderer_t *renderer, layout_box_t *box);
display_list_t *renderer_render_borders(renderer_t *renderer, layout_box_t *box);
display_list_t *renderer_render_outline(renderer_t *renderer, layout_box_t *box);

// Text rendering
display_list_t *renderer_render_text_runs(renderer_t *renderer, layout_box_t *box);
void renderer_shape_text(renderer_t *renderer, const char *text, os_font_t *font, os_point_t *positions, size_t count);
void renderer_measure_text(renderer_t *renderer, const char *text, os_font_t *font, os_size_t *size);

// Image rendering
os_texture_t *renderer_load_image(renderer_t *renderer, const char *path);
os_texture_t *renderer_load_image_from_data(renderer_t *renderer, const uint8_t *data, size_t size);
void renderer_cache_image(renderer_t *renderer, const char *key, os_texture_t *texture);
os_texture_t *renderer_get_cached_image(renderer_t *renderer, const char *key);

// Gradient rendering
os_gradient_t *renderer_create_linear_gradient(renderer_t *renderer, const os_point_t *start, const os_point_t *end);
os_gradient_t *renderer_create_radial_gradient(renderer_t *renderer, const os_point_t *center, float radius);
void renderer_add_gradient_stop(renderer_t *renderer, os_gradient_t *gradient, float offset, const os_color_t *color);

// Caching
display_list_t *renderer_get_cached_display_list(renderer_t *renderer, uint64_t hash);
void renderer_cache_display_list(renderer_t *renderer, uint64_t hash, display_list_t *dl);
void renderer_clear_cache(renderer_t *renderer);
void renderer_invalidate_cache(renderer_t *renderer, layout_box_t *box);

// Damage tracking
void renderer_add_damage(renderer_t *renderer, const os_rect_t *rect);
void renderer_add_damage_box(renderer_t *renderer, layout_box_t *box);
void renderer_clear_damage(renderer_t *renderer);
os_rect_t *renderer_get_damage_rects(renderer_t *renderer, size_t *count);
bool renderer_has_damage(renderer_t *renderer);

// Performance
void renderer_reset_metrics(renderer_t *renderer);
void renderer_start_frame_timer(renderer_t *renderer);
void renderer_end_frame_timer(renderer_t *renderer);
void renderer_report_performance(renderer_t *renderer);

// Debugging
void renderer_debug_display_list(display_list_t *dl);
void renderer_debug_render_tree(renderer_t *renderer);
void renderer_debug_cache_stats(renderer_t *renderer);
void renderer_debug_damage_rects(renderer_t *renderer);

// Utilities
os_color_t renderer_resolve_color(css_value_t *value, layout_context_t *ctx);
float renderer_resolve_length(css_value_t *value, layout_context_t *ctx, float reference);
os_path_t *renderer_create_border_path(layout_box_t *box);
os_path_t *renderer_create_outline_path(layout_box_t *box);
bool renderer_needs_redraw(renderer_t *renderer, layout_box_t *box);

// Animation support
typedef struct render_animation {
    char *property;
    css_value_t *from_value;
    css_value_t *to_value;
    double duration;
    char *timing_function;
    double delay;
    int iteration_count;
    char *direction;
    double current_time;
    bool is_running;
} render_animation_t;

render_animation_t *renderer_create_animation(renderer_t *renderer, const char *property, css_value_t *from, css_value_t *to);
void renderer_start_animation(renderer_t *renderer, render_animation_t *animation);
void renderer_update_animations(renderer_t *renderer, double delta_time);
css_value_t *renderer_interpolate_value(css_value_t *from, css_value_t *to, double progress);

// Selection rendering
void renderer_render_selection(renderer_t *renderer, layout_box_t *box, size_t start, size_t end);
void renderer_render_caret(renderer_t *renderer, layout_box_t *box, size_t position);
os_rect_t renderer_get_selection_rect(renderer_t *renderer, layout_box_t *box, size_t start, size_t end);

// Scrolling
void renderer_render_scrollbars(renderer_t *renderer, layout_box_t *box);
void renderer_render_scrollbar(renderer_t *renderer, layout_box_t *box, bool horizontal);
os_rect_t renderer_get_scrollbar_rect(layout_box_t *box, bool horizontal);
bool renderer_hit_test_scrollbar(layout_box_t *box, float x, float y, bool *horizontal);

// High DPI rendering
void renderer_set_dpi_scale(renderer_t *renderer, float scale);
float renderer_get_dpi_scale(renderer_t *renderer);
void renderer_scale_coordinates(renderer_t *renderer, float *x, float *y);
void renderer_scale_rect(renderer_t *renderer, os_rect_t *rect);

#endif // RENDER_RENDER_H
