# Modern OS Userland Stack Design

## Table of Contents
1. [libc Design](#libc-design)
2. [Graphics System Architecture](#graphics-system-architecture)
3. [Rendering Model](#rendering-model)
4. [HTML/CSS Engine Architecture](#htmlcss-engine-architecture)
5. [Event Loop and Concurrency Model](#event-loop-and-concurrency-model)
6. [API Design Principles](#api-design-principles)
7. [End-to-End Flow Examples](#end-to-end-flow-examples)

---

## libc Design

### POSIX Compatibility Scope

**Phase 1 (Core POSIX):**
- Basic file I/O: open, read, write, close, lseek, stat
- Process management: fork, execve, waitpid, exit
- Memory management: mmap, munmap, brk
- Signal handling: signal, sigaction, kill
- Thread support: pthread_create, pthread_join, pthread_mutex
- Socket I/O: socket, bind, listen, accept, connect, send, recv

**Phase 2 (Extended POSIX):**
- Extended file operations: fcntl, ioctl, select, poll
- Advanced threading: pthread_cond, pthread_rwlock
- Memory mapping with protection: mprotect
- Terminal I/O: termios
- User/group management: getuid, getgid, setuid, setgid

**Explicitly Excluded (for now):**
- System V IPC (msgctl, semctl, shmctl)
- POSIX real-time extensions
- POSIX threads scheduling policies
- Complex ACL systems

### Internal Architecture

```
+-------------------+
|   Public API      | (stdio, stdlib, string, etc.)
+-------------------+
|   POSIX Layer     | (pthreads, socket, signal wrappers)
+-------------------+
|   Syscall Layer   | (syscall interface, errno handling)
+-------------------+
|   Kernel Interface| (raw system calls)
+-------------------+
```

**Key Components:**

1. **Syscall Layer (`src/syscall/`)**
   - Unified syscall interface with error handling
   - Automatic errno setting
   - Architecture-specific syscall number mapping

2. **Memory Management (`src/memory/`)**
   - Custom malloc implementation (jemalloc-inspired)
   - Memory alignment and debugging support
   - Arena-based allocation for performance

3. **Threading (`src/threading/`)**
   - 1:1 threading model (kernel threads)
   - pthreads compatibility layer
   - Thread-local storage support

4. **I/O (`src/io/`)**
   - Buffered stdio implementation
   - File descriptor management
   - Socket abstraction layer

### Implementation Decisions

**malloc Strategy:**
- Use a modified jemalloc approach with size classes
- Separate arenas for different thread types
- Integration with kernel's mmap for large allocations
- Debug mode with guard pages and poisoning

**Threading Model:**
- 1:1 kernel threading (no green threads)
- Full pthreads compatibility
- futex-based synchronization primitives
- Thread cancellation support

**Linking Strategy:**
- Static linking by default for system binaries
- Dynamic linking support for user applications
- Versioned symbol exports for ABI stability
- Lazy binding for performance

### OS-Specific Features Integration

```c
// Example: OS-specific extensions in separate namespace
#include <os/ext.h>

// OS-specific graphics integration
os_surface_t *os_create_surface(uint32_t width, uint32_t height);
int os_bind_surface_to_window(os_surface_t *surface, os_window_t *window);

// OS-specific event handling
typedef struct os_event {
    uint32_t type;
    uint64_t timestamp;
    union {
        struct { int x, y; } mouse;
        struct { uint32_t keycode; } key;
        // ...
    } data;
} os_event_t;

int os_get_event(os_event_t *event, int timeout_ms);
```

---

## Graphics System Architecture

### Layered Design

```
+-------------------+
|   Application     | (C API, HTML/CSS Engine)
+-------------------+
|   Graphics API    | (surfaces, drawing primitives)
+-------------------+
|   Command Buffer  | (display lists, rendering commands)
+-------------------+
|   GPU Backend     | (resource management, command submission)
+-------------------+
|   Driver Interface| (hardware abstraction)
+-------------------+
```

### Low-level GPU Backend

**Resource Model:**
```c
typedef struct gpu_buffer {
    uint32_t id;
    size_t size;
    uint32_t usage;  // VERTEX, INDEX, UNIFORM, STORAGE
    void *mapped_ptr;
} gpu_buffer_t;

typedef struct gpu_texture {
    uint32_t id;
    uint32_t width, height;
    uint32_t format;  // RGBA8, RGB8, etc.
    uint32_t usage;   // SAMPLED, RENDER_TARGET, STORAGE
} gpu_texture_t;

typedef struct gpu_pipeline {
    uint32_t id;
    uint32_t vertex_shader;
    uint32_t fragment_shader;
    uint32_t blend_state;
    uint32_t raster_state;
} gpu_pipeline_t;
```

**Command Submission:**
```c
typedef struct gpu_cmd_buffer {
    uint32_t *commands;
    size_t capacity;
    size_t count;
    gpu_fence_t *signal_fence;
} gpu_cmd_buffer_t;

// Command types
enum {
    GPU_CMD_DRAW,
    GPU_CMD_DRAW_INDEXED,
    GPU_CMD_BIND_PIPELINE,
    GPU_CMD_BIND_VERTEX_BUFFER,
    GPU_CMD_BIND_INDEX_BUFFER,
    GPU_CMD_BIND_DESCRIPTOR_SET,
    GPU_CMD_SET_VIEWPORT,
    GPU_CMD_SET_SCISSOR,
    GPU_CMD_COPY_BUFFER,
    GPU_CMD_COPY_TEXTURE,
    GPU_CMD_BARRIER
};
```

**Synchronization:**
```c
typedef struct gpu_fence {
    uint32_t id;
    bool signaled;
    uint64_t timestamp;
} gpu_fence_t;

int gpu_submit_commands(gpu_cmd_buffer_t *cmd_buffer, gpu_fence_t **fence);
int gpu_wait_fence(gpu_fence_t *fence, uint64_t timeout_ns);
```

### Public Graphics API

**Surface and Window Abstraction:**
```c
typedef struct os_surface os_surface_t;
typedef struct os_window os_window_t;

// Window management
os_window_t *os_window_create(const char *title, uint32_t width, uint32_t height);
void os_window_destroy(os_window_t *window);
void os_window_show(os_window_t *window);
void os_window_hide(os_window_t *window);
void os_window_set_size(os_window_t *window, uint32_t width, uint32_t height);
void os_window_get_size(os_window_t *window, uint32_t *width, uint32_t *height);

// Surface management
os_surface_t *os_surface_create(uint32_t width, uint32_t height);
void os_surface_destroy(os_surface_t *surface);
void os_surface_resize(os_surface_t *surface, uint32_t width, uint32_t height);
```

**Drawing Primitives:**
```c
typedef struct os_color {
    float r, g, b, a;
} os_color_t;

typedef struct os_rect {
    float x, y, width, height;
} os_rect_t;

typedef struct os_point {
    float x, y;
} os_point_t;

// Drawing context
typedef struct os_paint_ctx os_paint_ctx_t;

os_paint_ctx_t *os_paint_begin(os_surface_t *surface);
void os_paint_end(os_paint_ctx_t *ctx);

// Primitives
void os_paint_fill_rect(os_paint_ctx_t *ctx, const os_rect_t *rect, const os_color_t *color);
void os_paint_stroke_rect(os_paint_ctx_t *ctx, const os_rect_t *rect, const os_color_t *color, float thickness);
void os_paint_fill_circle(os_paint_ctx_t *ctx, const os_point_t *center, float radius, const os_color_t *color);
void os_paint_draw_line(os_paint_ctx_t *ctx, const os_point_t *p1, const os_point_t *p2, const os_color_t *color, float thickness);

// Images and text
void os_paint_draw_image(os_paint_ctx_t *ctx, os_texture_t *texture, const os_rect_t *dest_rect);
void os_paint_draw_text(os_paint_ctx_t *ctx, const char *text, os_font_t *font, const os_point_t *pos, const os_color_t *color);
```

**Transforms and Clipping:**
```c
void os_paint_push_transform(os_paint_ctx_t *ctx, const float *matrix_4x4);
void os_paint_pop_transform(os_paint_ctx_t *ctx);
void os_paint_set_clip_rect(os_paint_ctx_t *ctx, const os_rect_t *clip);
void os_paint_clear_clip(os_paint_ctx_t *ctx);
```

**Frame Lifecycle:**
```c
void os_surface_begin_frame(os_surface_t *surface);
void os_surface_end_frame(os_surface_t *surface);
void os_surface_present(os_surface_t *surface);
```

### Compositor / Window Server

**Surface Management:**
```c
typedef struct compositor_surface {
    uint32_t id;
    os_surface_t *surface;
    os_window_t *window;
    os_rect_t bounds;
    float opacity;
    bool visible;
    uint32_t z_order;
} compositor_surface_t;

typedef struct compositor {
    compositor_surface_t **surfaces;
    size_t surface_count;
    os_surface_t *back_buffer;
    os_surface_t *front_buffer;
} compositor_t;
```

**Composition Model:**
```c
typedef struct damage_rect {
    os_rect_t rect;
    bool damaged;
} damage_rect_t;

void compositor_add_surface(compositor_t *comp, compositor_surface_t *surface);
void compositor_remove_surface(compositor_t *comp, uint32_t surface_id);
void compositor_move_surface(compositor_t *comp, uint32_t surface_id, const os_rect_t *new_bounds);
void compositor_set_surface_opacity(compositor_t *comp, uint32_t surface_id, float opacity);
void compositor_compose(compositor_t *comp, const damage_rect_t *damage_areas, size_t damage_count);
```

**Client-Server Protocol:**
```c
// IPC messages between compositor and clients
typedef enum {
    MSG_CREATE_WINDOW,
    MSG_DESTROY_WINDOW,
    MSG_SHOW_WINDOW,
    MSG_HIDE_WINDOW,
    MSG_MOVE_WINDOW,
    MSG_RESIZE_WINDOW,
    MSG_DAMAGE_SURFACE,
    MSG_PRESENT_SURFACE,
    MSG_INPUT_EVENT,
    MSG_FOCUS_CHANGE
} compositor_msg_type_t;

typedef struct compositor_msg {
    uint32_t client_id;
    compositor_msg_type_t type;
    size_t data_size;
    uint8_t data[];
} compositor_msg_t;
```

---

## Rendering Model

### Display List Architecture

```
Application -> Display List Builder -> Command Buffer -> GPU
                |                      |
                v                      v
            Layout Engine        Command Optimizer
                |                      |
                v                      v
            Style Resolver    Resource Manager
```

**Display List Structure:**
```c
typedef enum {
    DL_CLEAR,
    DL_FILL_RECT,
    DL_STROKE_RECT,
    DL_DRAW_IMAGE,
    DL_DRAW_TEXT,
    DL_PUSH_CLIP,
    DL_POP_CLIP,
    DL_PUSH_TRANSFORM,
    DL_POP_TRANSFORM,
    DL_DRAW_PATH,
    DL_DRAW_SHADOW
} display_list_op_t;

typedef struct display_list_cmd {
    display_list_op_t op;
    uint32_t data_size;
    uint8_t data[];
} display_list_cmd_t;

typedef struct display_list {
    display_list_cmd_t *commands;
    size_t command_count;
    size_t capacity;
    uint32_t ref_count;
} display_list_t;
```

### Layout and Rendering Separation

**Layout Phase:**
```c
typedef struct layout_box {
    os_rect_t content_rect;
    os_rect_t padding_rect;
    os_rect_t border_rect;
    os_rect_t margin_rect;
    float baseline;
    struct layout_box *parent;
    struct layout_box **children;
    size_t child_count;
} layout_box_t;

typedef struct layout_engine {
    layout_box_t *root_box;
    os_font_t *default_font;
    float dpi_scale;
} layout_engine_t;

layout_box_t *layout_engine_compute_layout(layout_engine_t *engine, const dom_node_t *root);
```

**Rendering Phase:**
```c
typedef struct render_context {
    os_paint_ctx_t *paint_ctx;
    display_list_t *display_list;
    os_rect_t viewport;
    os_rect_t clip_rect;
    float transform_matrix[16];
} render_context_t;

void render_layout_box(render_context_t *ctx, const layout_box_t *box, const style_t *style);
display_list_t *render_create_display_list(const layout_box_t *root_box);
```

### Partial Updates and Caching

**Damage Tracking:**
```c
typedef struct damage_tracker {
    os_rect_t *dirty_rects;
    size_t dirty_count;
    os_rect_t *cached_rects;
    size_t cached_count;
} damage_tracker_t;

void damage_tracker_invalidate_rect(damage_tracker_t *tracker, const os_rect_t *rect);
void damage_tracker_mark_clean(damage_tracker_t *tracker, const os_rect_t *rect);
bool damage_tracker_needs_redraw(const damage_tracker_t *tracker, const os_rect_t *rect);
```

**Rendering Cache:**
```c
typedef struct render_cache {
    hash_table_t *text_layout_cache;    // string -> text_layout
    hash_table_t *image_cache;          // path -> texture
    hash_table_t *gradient_cache;       // gradient_def -> texture
    hash_table_t *display_list_cache;   // subtree_hash -> display_list
} render_cache_t;

display_list_t *cache_get_display_list(render_cache_t *cache, uint64_t subtree_hash);
void cache_put_display_list(render_cache_t *cache, uint64_t subtree_hash, display_list_t *dl);
```

---

## HTML/CSS Engine Architecture

### Parser Strategy

**HTML Parser:**
```c
typedef enum {
    HTML_TOKEN_DOCTYPE,
    HTML_TOKEN_START_TAG,
    HTML_TOKEN_END_TAG,
    HTML_TOKEN_TEXT,
    HTML_TOKEN_COMMENT,
    HTML_TOKEN_EOF
} html_token_type_t;

typedef struct html_token {
    html_token_type_t type;
    char *tag_name;
    char **attributes;
    char **values;
    size_t attr_count;
    char *text;
} html_token_t;

typedef struct html_parser {
    const char *input;
    size_t position;
    html_token_t current_token;
    dom_node_t *current_node;
} html_parser_t;

dom_node_t *html_parser_parse(html_parser_t *parser, const char *html);
```

**CSS Parser:**
```c
typedef struct css_selector {
    char *tag_name;
    char **class_names;
    char **id_names;
    size_t class_count;
    size_t id_count;
    struct css_selector *next;  // for compound selectors
} css_selector_t;

typedef struct css_rule {
    css_selector_t *selectors;
    css_declaration_t *declarations;
    size_t declaration_count;
    uint32_t specificity;
} css_rule_t;

typedef struct css_parser {
    const char *input;
    size_t position;
    css_rule_t **rules;
    size_t rule_count;
} css_parser_t;

css_stylesheet_t *css_parser_parse(css_parser_t *parser, const char *css);
```

### Style Resolution

**Style System:**
```c
typedef struct css_property {
    enum {
        CSS_PROP_COLOR,
        CSS_PROP_BACKGROUND_COLOR,
        CSS_PROP_WIDTH,
        CSS_PROP_HEIGHT,
        CSS_PROP_MARGIN,
        CSS_PROP_PADDING,
        CSS_PROP_FONT_FAMILY,
        CSS_PROP_FONT_SIZE,
        // ...
    } type;
    union {
        os_color_t color;
        float length;
        char *string;
    } value;
    bool important;
} css_property_t;

typedef struct computed_style {
    css_property_t *properties;
    size_t property_count;
    os_font_t *font;
    float line_height;
} computed_style_t;

computed_style_t *style_resolve_for_node(dom_node_t *node, css_stylesheet_t *stylesheet);
```

### Layout System

**Box Model Implementation:**
```c
typedef struct box_model {
    float margin_top, margin_right, margin_bottom, margin_left;
    float padding_top, padding_right, padding_bottom, padding_left;
    float border_top, border_right, border_bottom, border_left;
    float width, height;
    float min_width, min_height;
    float max_width, max_height;
} box_model_t;

typedef enum {
    LAYOUT_BLOCK,
    LAYOUT_INLINE,
    LAYOUT_INLINE_BLOCK,
    LAYOUT_FLEX,
    LAYOUT_GRID
} layout_type_t;

typedef struct layout_context {
    float available_width;
    float available_height;
    layout_type_t layout_type;
    bool is_root;
} layout_context_t;

layout_box_t *layout_compute_block_box(layout_context_t *ctx, dom_node_t *node, computed_style_t *style);
layout_box_t *layout_compute_inline_box(layout_context_t *ctx, dom_node_t *node, computed_style_t *style);
```

### Text Layout

**Text Shaping and Layout:**
```c
typedef struct glyph {
    uint32_t codepoint;
    float x, y;
    float width, height;
    float advance_x, advance_y;
} glyph_t;

typedef struct text_run {
    glyph_t *glyphs;
    size_t glyph_count;
    os_font_t *font;
    float font_size;
    os_color_t color;
    float x, y;
} text_run_t;

typedef struct text_layout_engine {
    harfbuzz_t *hb_font;
    os_font_t *system_font;
    hash_table_t *shape_cache;
} text_layout_engine_t;

text_run_t *text_layout_shape_text(text_layout_engine_t *engine, const char *text, os_font_t *font, float font_size);
```

### Rendering Integration

**HTML to Display List:**
```c
typedef struct html_renderer {
    render_context_t *render_ctx;
    layout_engine_t *layout_engine;
    text_layout_engine_t *text_engine;
    render_cache_t *cache;
} html_renderer_t;

display_list_t *html_renderer_render_node(html_renderer_t *renderer, dom_node_t *node, const os_rect_t *viewport);
void html_renderer_invalidate_node(html_renderer_t *renderer, dom_node_t *node);
```

### Event System

**DOM Event Handling:**
```c
typedef enum {
    DOM_EVENT_CLICK,
    DOM_EVENT_MOUSEDOWN,
    DOM_EVENT_MOUSEUP,
    DOM_EVENT_MOUSEMOVE,
    DOM_EVENT_KEYDOWN,
    DOM_EVENT_KEYUP,
    DOM_EVENT_FOCUS,
    DOM_EVENT_BLUR,
    DOM_EVENT_LOAD,
    DOM_EVENT_RESIZE
} dom_event_type_t;

typedef struct dom_event {
    dom_event_type_t type;
    dom_node_t *target;
    dom_node_t *current_target;
    uint64_t timestamp;
    bool bubbles;
    bool cancelable;
    union {
        struct { int x, y, button; } mouse;
        struct { uint32_t keycode, modifiers; } key;
    } data;
} dom_event_t;

typedef void (*dom_event_handler_t)(dom_event_t *event, void *user_data);

void dom_add_event_listener(dom_node_t *node, dom_event_type_t type, dom_event_handler_t handler, void *user_data);
void dom_remove_event_listener(dom_node_t *node, dom_event_type_t type, dom_event_handler_t handler);
void dom_dispatch_event(dom_node_t *node, dom_event_t *event);
```

### Supported Features (Phase 1)

**HTML Support:**
- Basic HTML5 tags: div, span, p, h1-h6, a, img, button, input, textarea
- Document structure: html, head, body, title, meta, link
- Forms: form, label, select, option
- Tables: table, tr, td, th, thead, tbody

**CSS Support:**
- Box model: margin, padding, border, width, height
- Typography: font-family, font-size, font-weight, color, line-height
- Layout: display (block, inline, inline-block), position (static, relative, absolute)
- Backgrounds: background-color, background-image
- Selectors: tag, class, ID, descendant, child

**Deferred Features (Phase 2):**
- Flexbox and Grid layout
- CSS animations and transitions
- Complex selectors (pseudo-classes, pseudo-elements)
- CSS custom properties
- Shadow DOM
- Web Components

---

## Event Loop and Concurrency Model

### Unified Event Loop Structure

```c
typedef enum {
    EVENT_TYPE_WINDOW,
    EVENT_TYPE_INPUT,
    EVENT_TYPE_TIMER,
    EVENT_TYPE_NETWORK,
    EVENT_TYPE_RENDER,
    EVENT_TYPE_USER,
    EVENT_TYPE_JS_CALLBACK
} event_type_t;

typedef struct event {
    event_type_t type;
    uint64_t timestamp;
    uint32_t source_id;
    union {
        struct { int width, height; } window_resize;
        struct { int x, y, button; } mouse_click;
        struct { uint32_t keycode; } key_press;
        struct { void (*callback)(void *); void *data; } timer;
        struct { int fd, events; } network;
        struct { os_surface_t *surface; } render;
        struct { void *data; } user;
    } data;
} event_t;

typedef struct event_loop {
    event_t *event_queue;
    size_t queue_size;
    size_t queue_capacity;
    size_t queue_head;
    size_t queue_tail;
    
    // Timer management
    struct timer *timers;
    size_t timer_count;
    
    // File descriptor monitoring
    struct pollfd *poll_fds;
    size_t poll_fd_count;
    
    // Render scheduling
    bool render_pending;
    uint64_t last_render_time;
    
    // JS integration
    struct js_runtime *js_runtime;
    
    bool running;
} event_loop_t;
```

### Event Loop Implementation

```c
event_loop_t *event_loop_create(void);
void event_loop_destroy(event_loop_t *loop);

int event_loop_run(event_loop_t *loop);
int event_loop_stop(event_loop_t *loop);

// Event posting
void event_loop_post_event(event_loop_t *loop, const event_t *event);
void event_loop_post_timer(event_loop_t *loop, uint64_t delay_ms, void (*callback)(void *), void *data);
void event_loop_post_render(event_loop_t *loop, os_surface_t *surface);

// File descriptor monitoring
void event_loop_add_fd(event_loop_t *loop, int fd, int events, void (*callback)(int, int, void *), void *data);
void event_loop_remove_fd(event_loop_t *loop, int fd);
```

### Application Integration

**Native C Application:**
```c
typedef struct app_context {
    event_loop_t *event_loop;
    os_window_t *window;
    os_surface_t *surface;
    bool running;
} app_context_t;

// Application callbacks
typedef void (*app_init_cb_t)(app_context_t *app);
typedef void (*app_event_cb_t)(app_context_t *app, const event_t *event);
typedef void (*app_render_cb_t)(app_context_t *app, os_paint_ctx_t *ctx);
typedef void (*app_cleanup_cb_t)(app_context_t *app);

int app_run(const char *title, uint32_t width, uint32_t height,
            app_init_cb_t init, app_event_cb_t event_handler, 
            app_render_cb_t render, app_cleanup_cb_t cleanup);
```

### Compositor Integration

```c
typedef struct compositor_client {
    uint32_t client_id;
    int socket_fd;
    os_surface_t **surfaces;
    size_t surface_count;
    event_loop_t *event_loop;
} compositor_client_t;

// Compositor event loop integration
void compositor_handle_client_message(compositor_client_t *client, const event_t *event);
void compositor_schedule_composition(compositor_client_t *client);
void compositor_handle_input_event(compositor_client_t *client, const event_t *event);
```

### JavaScript Integration

**V8 Integration:**
```c
typedef struct js_runtime {
    v8::Isolate *isolate;
    v8::Global<v8::Context> context;
    v8::Global<v8::Function> event_handler;
    event_loop_t *event_loop;
} js_runtime_t;

js_runtime_t *js_runtime_create(event_loop_t *loop);
void js_runtime_destroy(js_runtime_t *runtime);

// JS to C bridge
void js_runtime_post_event(js_runtime_t *runtime, const event_t *event);
void js_runtime_set_event_handler(js_runtime_t *runtime, v8::Local<v8::Function> handler);

// C to JS bridge
void js_runtime_call_function(js_runtime_t *runtime, const char *function_name, 
                             const v8::Local<v8::Value> *args, size_t arg_count);
```

**Event Loop Integration:**
```c
// JS timer functions
void js_set_timer(js_runtime_t *runtime, uint64_t delay_ms, v8::Local<v8::Function> callback);
void js_clear_timer(js_runtime_t *runtime, uint32_t timer_id);

// JS async operations
void js_async_operation(js_runtime_t *runtime, const char *operation, 
                       v8::Local<v8::Function> resolve, v8::Local<v8::Function> reject);

// JS rendering integration
void js_request_animation_frame(js_runtime_t *runtime, v8::Local<v8::Function> callback);
```

---

## API Design Principles

### Memory Ownership Rules

1. **Create/Destroy Pattern:** Every `create` function has a corresponding `destroy` function
   ```c
   os_surface_t *os_surface_create(uint32_t width, uint32_t height);
   void os_surface_destroy(os_surface_t *surface);
   ```

2. **Borrowed References:** Function parameters are borrowed, not owned
   ```c
   void os_paint_fill_rect(os_paint_ctx_t *ctx, const os_rect_t *rect, const os_color_t *color);
   // ctx, rect, and color are borrowed for the duration of the call
   ```

3. **Transfer of Ownership:** Functions with "take" in the name take ownership
   ```c
   void compositor_take_surface(compositor_t *comp, os_surface_t *surface);
   // surface is now owned by the compositor
   ```

4. **Reference Counting:** Objects that are shared use reference counting
   ```c
   display_list_t *display_list_ref(display_list_t *dl);
   void display_list_unref(display_list_t *dl);
   ```

### Error Handling Strategy

```c
// Return codes
typedef enum {
    OS_SUCCESS = 0,
    OS_ERROR_INVALID_PARAM = -1,
    OS_ERROR_OUT_OF_MEMORY = -2,
    OS_ERROR_RESOURCE_BUSY = -3,
    OS_ERROR_TIMEOUT = -4,
    OS_ERROR_NOT_FOUND = -5,
    OS_ERROR_PERMISSION_DENIED = -6
} os_result_t;

// Error handling pattern
os_result_t os_function_with_error(param1, param2, os_error_t *out_error);

typedef struct os_error {
    os_result_t code;
    char message[256];
    char file[64];
    int line;
} os_error_t;
```

### Opaque Handles vs Structs

**Use Opaque Handles For:**
- Resource objects (surfaces, textures, buffers)
- Internal state objects (render contexts, layout engines)
- Objects with complex internal state

```c
typedef struct os_surface os_surface_t;  // Opaque
typedef struct os_paint_ctx os_paint_ctx_t;  // Opaque
```

**Use Public Structs For:**
- Simple data containers (points, rects, colors)
- Configuration structures
- Events passed between components

```c
typedef struct os_point {
    float x, y;
} os_point_t;  // Public

typedef struct os_rect {
    float x, y, width, height;
} os_rect_t;  // Public
```

### Thread Safety Expectations

1. **Thread-Safe Objects:** Objects that can be used from multiple threads are explicitly marked
   ```c
   // Thread-safe reference counting
   display_list_t *display_list_ref_threadsafe(display_list_t *dl);
   void display_list_unref_threadsafe(display_list_t *dl);
   ```

2. **Thread-Local Objects:** Objects that are not thread-safe are documented as such
   ```c
   // NOT thread-safe - must be used from the creating thread only
   os_paint_ctx_t *os_paint_begin(os_surface_t *surface);
   ```

3. **Main Thread Requirements:** Graphics and UI operations must be performed on the main thread
   ```c
   // Must be called from the main thread
   os_window_t *os_window_create(const char *title, uint32_t width, uint32_t height);
   ```

### Naming Conventions

```c
// Namespace prefix for all public APIs
os_*
compositor_*
html_*
css_*
js_*

// Function naming: noun_verb pattern
os_surface_create()
os_surface_destroy()
os_surface_resize()

// Struct naming: snake_case
typedef struct os_surface os_surface_t;
typedef struct compositor_client compositor_client_t;

// Enum naming: UPPER_CASE with prefix
typedef enum {
    OS_ERROR_SUCCESS,
    OS_ERROR_INVALID_PARAM,
    OS_ERROR_OUT_OF_MEMORY
} os_result_t;
```

### JavaScript Binding Considerations

**API Design for JS Bindings:**
1. **Simple Parameter Types:** Use basic types that map well to JS
2. **Explicit Resource Management:** Avoid RAII patterns that don't translate well
3. **Callback-Based Async:** Use callbacks instead of promises initially
4. **Error Propagation:** Use result codes instead of exceptions

```c
// JS-friendly API design
typedef void (*js_async_callback_t)(os_result_t result, void *data, void *user_data);

os_result_t os_load_image_async(const char *path, js_async_callback_t callback, void *user_data);
```

---

## End-to-End Flow Examples

### Native C Application Flow

**1. Application Initialization**
```c
#include <os/window.h>
#include <os/graphics.h>
#include <os/events.h>

typedef struct app_state {
    os_window_t *window;
    os_surface_t *surface;
    os_color_t bg_color;
    bool running;
} app_state_t;

void app_init(app_state_t *app) {
    // Create window
    app->window = os_window_create("My App", 800, 600);
    
    // Create surface for rendering
    app->surface = os_surface_create(800, 600);
    
    // Bind surface to window
    os_window_bind_surface(app->window, app->surface);
    
    // Show window
    os_window_show(app->window);
    
    // Initialize state
    app->bg_color = (os_color_t){0.2f, 0.3f, 0.8f, 1.0f};
    app->running = true;
}
```

**2. Main Event Loop**
```c
void app_handle_events(app_state_t *app) {
    event_t event;
    
    while (app->running && event_loop_get_next_event(&event, 0)) {
        switch (event.type) {
            case EVENT_TYPE_WINDOW:
                if (event.data.window_resize.width != 0) {
                    // Handle window resize
                    os_surface_resize(app->surface, 
                                     event.data.window_resize.width,
                                     event.data.window_resize.height);
                }
                break;
                
            case EVENT_TYPE_INPUT:
                if (event.data.mouse_click.button == 1) {
                    // Handle mouse click
                    app->bg_color.r = (float)rand() / RAND_MAX;
                    app->bg_color.g = (float)rand() / RAND_MAX;
                    app->bg_color.b = (float)rand() / RAND_MAX;
                }
                break;
                
            case EVENT_TYPE_RENDER:
                app_render(app);
                break;
        }
    }
}
```

**3. Rendering**
```c
void app_render(app_state_t *app) {
    // Begin frame
    os_surface_begin_frame(app->surface);
    
    // Get paint context
    os_paint_ctx_t *ctx = os_paint_begin(app->surface);
    
    // Clear background
    os_rect_t full_rect = {0, 0, 800, 600};
    os_paint_fill_rect(ctx, &full_rect, &app->bg_color);
    
    // Draw some content
    os_rect_t box_rect = {100, 100, 200, 150};
    os_color_t box_color = {1.0f, 1.0f, 1.0f, 0.8f};
    os_paint_fill_rect(ctx, &box_rect, &box_color);
    
    // Draw text
    os_point_t text_pos = {120, 130};
    os_color_t text_color = {0.0f, 0.0f, 0.0f, 1.0f};
    os_font_t *font = os_font_get_default();
    os_paint_draw_text(ctx, "Hello, World!", font, &text_pos, &text_color);
    
    // End painting
    os_paint_end(ctx);
    
    // End frame and present
    os_surface_end_frame(app->surface);
    os_surface_present(app->surface);
}
```

**4. Cleanup**
```c
void app_cleanup(app_state_t *app) {
    os_surface_destroy(app->surface);
    os_window_destroy(app->window);
}
```

### HTML/CSS Engine Flow

**1. HTML Document Loading**
```c
typedef struct html_app {
    os_window_t *window;
    os_surface_t *surface;
    html_renderer_t *renderer;
    dom_document_t *document;
    css_stylesheet_t *stylesheet;
    layout_box_t *layout_root;
    bool needs_relayout;
    bool needs_rerender;
} html_app_t;

void html_app_load_document(html_app_t *app, const char *html_path, const char *css_path) {
    // Load HTML
    char *html_content = read_file(html_path);
    html_parser_t *html_parser = html_parser_create();
    app->document = html_parser_parse(html_parser, html_content);
    html_parser_destroy(html_parser);
    free(html_content);
    
    // Load CSS
    char *css_content = read_file(css_path);
    css_parser_t *css_parser = css_parser_create();
    app->stylesheet = css_parser_parse(css_parser, css_content);
    css_parser_destroy(css_parser);
    free(css_content);
    
    // Mark for relayout
    app->needs_relayout = true;
    app->needs_rerender = true;
}
```

**2. Layout Computation**
```c
void html_app_compute_layout(html_app_t *app) {
    if (!app->needs_relayout) return;
    
    // Get viewport size
    uint32_t width, height;
    os_window_get_size(app->window, &width, &height);
    
    // Create layout context
    layout_context_t ctx = {
        .available_width = (float)width,
        .available_height = (float)height,
        .layout_type = LAYOUT_BLOCK,
        .is_root = true
    };
    
    // Compute layout for root
    dom_node_t *root = dom_document_get_root(app->document);
    computed_style_t *root_style = style_resolve_for_node(root, app->stylesheet);
    app->layout_root = layout_compute_block_box(&ctx, root, root_style);
    
    app->needs_relayout = false;
}
```

**3. Rendering Pipeline**
```c
void html_app_render(html_app_t *app) {
    if (!app->needs_rerender) return;
    
    // Ensure layout is up to date
    html_app_compute_layout(app);
    
    // Begin frame
    os_surface_begin_frame(app->surface);
    
    // Get viewport
    uint32_t width, height;
    os_window_get_size(app->window, &width, &height);
    os_rect_t viewport = {0, 0, (float)width, (float)height};
    
    // Render to display list
    display_list_t *display_list = html_renderer_render_node(app->renderer, 
                                                           dom_document_get_root(app->document),
                                                           &viewport);
    
    // Execute display list
    os_paint_ctx_t *ctx = os_paint_begin(app->surface);
    display_list_execute(ctx, display_list);
    os_paint_end(ctx);
    
    // Cleanup
    display_list_unref(display_list);
    
    // End frame and present
    os_surface_end_frame(app->surface);
    os_surface_present(app->surface);
    
    app->needs_rerender = false;
}
```

**4. Event Handling**
```c
void html_app_handle_input(html_app_t *app, const event_t *event) {
    if (event->type != EVENT_TYPE_INPUT) return;
    
    // Convert to DOM event
    dom_event_t dom_event = {
        .type = DOM_EVENT_CLICK,
        .target = NULL,  // Will be determined by hit testing
        .timestamp = event->timestamp,
        .bubbles = true,
        .cancelable = true
    };
    
    if (event->data.mouse_click.button == 1) {
        dom_event.data.mouse.x = event->data.mouse_click.x;
        dom_event.data.mouse.y = event->data.mouse_click.y;
        
        // Hit testing to find target
        dom_node_t *target = html_renderer_hit_test(app->renderer, 
                                                   event->data.mouse_click.x,
                                                   event->data.mouse_click.y);
        if (target) {
            dom_event.target = target;
            dom_dispatch_event(target, &dom_event);
            
            // Check if event caused style/layout changes
            if (dom_event_caused_style_change(&dom_event)) {
                app->needs_relayout = true;
                app->needs_rerender = true;
            }
        }
    }
}
```

**5. JavaScript Integration**
```c
void html_app_init_js(html_app_t *app) {
    // Create JS runtime
    app->renderer->js_runtime = js_runtime_create(app->renderer->event_loop);
    
    // Set up DOM bindings
    js_runtime_bind_dom_api(app->renderer->js_runtime, app->document);
    
    // Set event handler
    v8::Local<v8::Function> handler = js_compile_function(app->renderer->js_runtime,
                                                           "function(event) { console.log('Event:', event.type); }");
    js_runtime_set_event_handler(app->renderer->js_runtime, handler);
    
    // Load and execute scripts
    dom_node_list_t *scripts = dom_get_elements_by_tag_name(app->document, "script");
    for (size_t i = 0; i < scripts->length; i++) {
        dom_node_t *script = scripts->items[i];
        char *script_content = dom_get_text_content(script);
        js_runtime_execute(app->renderer->js_runtime, script_content);
        free(script_content);
    }
}
```

### Complete Application Flow

**Native C App:**
```
1. app_init() -> Create window, surface, show window
2. event_loop_run() -> Enter main loop
3. app_handle_events() -> Process input/window events
4. app_render() -> Draw to surface, present
5. app_cleanup() -> Destroy resources
```

**HTML/CSS App:**
```
1. html_app_load_document() -> Parse HTML/CSS
2. html_app_compute_layout() -> Layout boxes
3. html_app_render() -> Create display list, execute, present
4. html_app_handle_input() -> Convert to DOM events, dispatch
5. JS integration -> Execute callbacks, potentially trigger reflow
```

This design provides a comprehensive foundation for a modern OS userland stack with clear separation of concerns, clean APIs, and good integration points for both native and web-based applications.
