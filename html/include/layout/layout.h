#ifndef LAYOUT_LAYOUT_H
#define LAYOUT_LAYOUT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <dom/dom.h>
#include <css/css.h>
#include <os/graphics.h>

// Layout box types
typedef enum {
    LAYOUT_BOX_BLOCK,
    LAYOUT_BOX_INLINE,
    LAYOUT_BOX_INLINE_BLOCK,
    LAYOUT_BOX_FLEX,
    LAYOUT_BOX_GRID,
    LAYOUT_BOX_TABLE,
    LAYOUT_BOX_TABLE_ROW,
    LAYOUT_BOX_TABLE_CELL,
    LAYOUT_BOX_TEXT,
    LAYOUT_BOX_REPLACED,
    LAYOUT_BOX_ABSOLUTE,
    LAYOUT_BOX_FIXED
} layout_box_type_t;

// Layout box
typedef struct layout_box {
    layout_box_type_t type;
    
    // Associated DOM node
    dom_node_t *node;
    css_computed_style_t *style;
    
    // Geometry
    os_rect_t content_rect;
    os_rect_t padding_rect;
    os_rect_t border_rect;
    os_rect_t margin_rect;
    
    // Positioning
    float x, y;
    float width, height;
    float min_width, min_height;
    float max_width, max_height;
    
    // Margins, padding, borders
    float margin_top, margin_right, margin_bottom, margin_left;
    float padding_top, padding_right, padding_bottom, padding_left;
    float border_top_width, border_right_width, border_bottom_width, border_left_width;
    
    // Text layout (for text boxes)
    char *text;
    os_font_t *font;
    float font_size;
    float line_height;
    os_point_t *glyph_positions;
    size_t glyph_count;
    size_t line_count;
    float *line_widths;
    float *line_heights;
    
    // Children
    struct layout_box *first_child;
    struct layout_box *last_child;
    struct layout_box *next_sibling;
    struct layout_box *prev_sibling;
    struct layout_box *parent;
    
    // Layout state
    bool needs_layout;
    bool needs_paint;
    bool is_dirty;
    
    // Overflow
    bool has_overflow;
    os_rect_t overflow_rect;
    
    // Z-index and stacking context
    int z_index;
    bool creates_stacking_context;
    struct layout_box *stacking_context_root;
    
    // Positioning context
    bool is_positioned;
    bool is_absolute_positioned;
    bool is_fixed_positioned;
    os_point_t containing_block_offset;
    
    // Flexbox layout data
    struct {
        float flex_grow, flex_shrink, flex_basis;
        float main_size, cross_size;
        float main_position, cross_position;
        bool is_flex_item;
        bool is_flex_container;
    } flexbox;
    
    // Grid layout data
    struct {
        int grid_row_start, grid_row_end;
        int grid_column_start, grid_column_end;
        float grid_area_width, grid_area_height;
        bool is_grid_item;
        bool is_grid_container;
    } grid;
    
    // Table layout data
    struct {
        int table_row, table_column;
        int row_span, col_span;
        bool is_table_cell;
        bool is_table_row;
        bool is_table;
    } table;
    
    // Reference counting
    uint32_t ref_count;
} layout_box_t;

// Layout context
typedef struct layout_context {
    os_rect_t viewport;
    float dpi_scale;
    float font_scale;
    
    // Root layout box
    layout_box_t *root_box;
    
    // Layout state
    bool needs_relayout;
    bool needs_repaint;
    
    // Containing block chain
    layout_box_t **containing_blocks;
    size_t containing_block_count;
    
    // Stacking contexts
    layout_box_t **stacking_contexts;
    size_t stacking_context_count;
    
    // Viewport dimensions
    float viewport_width, viewport_height;
    float scroll_x, scroll_y;
    
    // Layout constraints
    float min_content_width, max_content_width;
    float min_content_height, max_content_height;
    
    // Line layout cache
    struct {
        char *text;
        os_font_t *font;
        float font_size;
        float width;
        os_point_t *positions;
        size_t glyph_count;
    } *line_cache;
    size_t line_cache_size;
    size_t line_cache_capacity;
} layout_context_t;

// Layout engine
typedef struct layout_engine {
    layout_context_t *context;
    
    // Layout algorithms
    layout_box_t *(*layout_block)(layout_context_t *ctx, layout_box_t *box);
    layout_box_t *(*layout_inline)(layout_context_t *ctx, layout_box_t *box);
    layout_box_t *(*layout_flex)(layout_context_t *ctx, layout_box_t *box);
    layout_box_t *(*layout_grid)(layout_context_t *ctx, layout_box_t *box);
    layout_box_t *(*layout_table)(layout_context_t *ctx, layout_box_t *box);
    
    // Text layout engine
    struct {
        os_font_t *default_font;
        float default_font_size;
        float line_height_multiplier;
    } text_engine;
    
    // Performance metrics
    struct {
        uint64_t layout_time_ms;
        uint64_t paint_time_ms;
        size_t box_count;
        size_t text_box_count;
        size_t reflow_count;
    } metrics;
} layout_engine_t;

// Layout box creation and management
layout_box_t *layout_box_create(layout_box_type_t type, dom_node_t *node);
void layout_box_destroy(layout_box_t *box);
layout_box_t *layout_box_ref(layout_box_t *box);
void layout_box_unref(layout_box_t *box);
void layout_box_add_child(layout_box_t *parent, layout_box_t *child);
void layout_box_remove_child(layout_box_t *parent, layout_box_t *child);
void layout_box_insert_before(layout_box_t *parent, layout_box_t *new_child, layout_box_t *reference_child);
void layout_box_insert_after(layout_box_t *parent, layout_box_t *new_child, layout_box_t *reference_child);

// Layout context management
layout_context_t *layout_context_create(float width, float height, float dpi_scale);
void layout_context_destroy(layout_context_t *context);
void layout_context_set_viewport(layout_context_t *context, float width, float height);
void layout_context_set_scroll(layout_context_t *context, float scroll_x, float scroll_y);
void layout_context_invalidate(layout_context_t *context);
void layout_context_invalidate_subtree(layout_context_t *context, layout_box_t *box);

// Layout engine management
layout_engine_t *layout_engine_create(layout_context_t *context);
void layout_engine_destroy(layout_engine_t *engine);
layout_box_t *layout_engine_build_layout(layout_engine_t *engine, dom_node_t *root, css_stylesheet_t *stylesheet);
void layout_engine_update_layout(layout_engine_t *engine, layout_box_t *box);
void layout_engine_invalidate_layout(layout_engine_t *engine, layout_box_t *box);

// Layout algorithms
layout_box_t *layout_block_box(layout_context_t *ctx, layout_box_t *box);
layout_box_t *layout_inline_box(layout_context_t *ctx, layout_box_t *box);
layout_box_t *layout_flex_box(layout_context_t *ctx, layout_box_t *box);
layout_box_t *layout_grid_box(layout_context_t *ctx, layout_box_t *box);
layout_box_t *layout_table_box(layout_context_t *ctx, layout_box_t *box);
layout_box_t *layout_text_box(layout_context_t *ctx, layout_box_t *box);
layout_box_t *layout_replaced_box(layout_context_t *ctx, layout_box_t *box);

// Block layout
float layout_block_measure_width(layout_context_t *ctx, layout_box_t *box);
float layout_block_measure_height(layout_context_t *ctx, layout_box_t *box);
void layout_block_position_children(layout_context_t *ctx, layout_box_t *box);
float layout_block_get_shrink_to_fit_width(layout_context_t *ctx, layout_box_t *box);

// Inline layout
void layout_inline_build_lines(layout_context_t *ctx, layout_box_t *box);
float layout_inline_measure_width(layout_context_t *ctx, layout_box_t *box);
float layout_inline_measure_height(layout_context_t *ctx, layout_box_t *box);
void layout_inline_position_inline_boxes(layout_context_t *ctx, layout_box_t *box);

// Flexbox layout
void layout_flexbox_distribute_items(layout_context_t *ctx, layout_box_t *box);
void layout_flexbox_resolve_flexible_lengths(layout_context_t *ctx, layout_box_t *box);
void layout_flexbox_position_items(layout_context_t *ctx, layout_box_t *box);
float layout_flexbox_measure_main_size(layout_context_t *ctx, layout_box_t *box);
float layout_flexbox_measure_cross_size(layout_context_t *ctx, layout_box_t *box);

// Grid layout
void layout_grid_define_tracks(layout_context_t *ctx, layout_box_t *box);
void layout_grid_place_items(layout_context_t *ctx, layout_box_t *box);
void layout_grid_position_items(layout_context_t *ctx, layout_box_t *box);
float layout_grid_measure_width(layout_context_t *ctx, layout_box_t *box);
float layout_grid_measure_height(layout_context_t *ctx, layout_box_t *box);

// Table layout
void layout_table_calculate_column_widths(layout_context_t *ctx, layout_box_t *box);
void layout_table_position_cells(layout_context_t *ctx, layout_box_t *box);
float layout_table_measure_width(layout_context_t *ctx, layout_box_t *box);
float layout_table_measure_height(layout_context_t *ctx, layout_box_t *box);

// Text layout
void layout_text_measure(layout_context_t *ctx, layout_box_t *box);
void layout_text_shape(layout_context_t *ctx, layout_box_t *box);
void layout_text_position_glyphs(layout_context_t *ctx, layout_box_t *box);
float layout_text_get_width(layout_context_t *ctx, layout_box_t *box);
float layout_text_get_height(layout_context_t *ctx, layout_box_t *box);
size_t layout_text_get_character_at_position(layout_context_t *ctx, layout_box_t *box, float x, float y);

// Replaced element layout
void layout_replaced_measure(layout_context_t *ctx, layout_box_t *box);
float layout_replaced_get_intrinsic_width(layout_context_t *ctx, layout_box_t *box);
float layout_replaced_get_intrinsic_height(layout_context_t *ctx, layout_box_t *box);
float layout_replaced_get_intrinsic_aspect_ratio(layout_context_t *ctx, layout_box_t *box);

// Positioning and overflow
void layout_position_absolute_boxes(layout_context_t *ctx, layout_box_t *container);
void layout_position_fixed_boxes(layout_context_t *ctx, layout_box_t *container);
void layout_handle_overflow(layout_context_t *ctx, layout_box_t *box);
layout_box_t *layout_get_containing_block(layout_context_t *ctx, layout_box_t *box);

// Stacking context
void layout_build_stacking_context(layout_context_t *ctx, layout_box_t *box);
layout_box_t **layout_get_stacking_context_order(layout_context_t *ctx, layout_box_t *root, size_t *count);
void layout_paint_stacking_context(layout_context_t *ctx, layout_box_t *box, os_paint_ctx_t *paint_ctx);

// Hit testing
layout_box_t *layout_hit_test(layout_context_t *ctx, layout_box_t *root, float x, float y);
layout_box_t *layout_hit_test_stacking_context(layout_context_t *ctx, layout_box_t *root, float x, float y);
bool layout_box_contains_point(layout_box_t *box, float x, float y);

// Layout debugging
void layout_debug_print_tree(layout_context_t *ctx, layout_box_t *root, int depth);
void layout_debug_print_box(layout_box_t *box, int depth);
void layout_debug_validate_layout(layout_context_t *ctx, layout_box_t *root);
void layout_debug_dump_metrics(layout_engine_t *engine);

// Layout utilities
bool layout_box_is_block_level(layout_box_t *box);
bool layout_box_is_inline_level(layout_box_t *box);
bool layout_box_is_positioned(layout_box_t *box);
bool layout_box_creates_stacking_context(layout_box_t *box);
bool layout_box_is_flex_container(layout_box_t *box);
bool layout_box_is_grid_container(layout_box_t *box);
bool layout_box_is_table_container(layout_box_t *box);

float layout_resolve_length(css_value_t *value, layout_context_t *ctx, layout_box_t *containing_block);
float layout_resolve_percentage(css_value_t *value, float reference);
os_color_t layout_resolve_color(css_value_t *value, layout_context_t *ctx, layout_box_t *containing_block);

// Layout performance
void layout_performance_reset_metrics(layout_engine_t *engine);
void layout_performance_start_timer(layout_engine_t *engine);
void layout_performance_stop_timer(layout_engine_t *engine);
void layout_performance_report(layout_engine_t *engine);

// Layout validation
bool layout_validate_box_tree(layout_box_t *root);
bool layout_validate_box(layout_box_t *box);
bool layout_validate_geometry(layout_box_t *box);
bool layout_validate_parent_child_relationships(layout_box_t *box);

#endif // LAYOUT_LAYOUT_H
