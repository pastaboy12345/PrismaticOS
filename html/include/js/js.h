#ifndef JS_JS_H
#define JS_JS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <dom/dom.h>
#include <css/css.h>
#include <layout/layout.h>
#include <render/render.h>

// V8 integration
#include <v8.h>

// JavaScript runtime
typedef struct js_runtime {
    v8::Isolate *isolate;
    v8::Global<v8::Context> context;
    
    // Event loop integration
    void *event_loop;
    
    // DOM bindings
    v8::Global<v8::Object> global_dom;
    v8::Global<v8::Object> global_window;
    v8::Global<v8::Object> global_document;
    v8::Global<v8::Object> global_console;
    
    // Module system
    v8::Global<v8::Object> module_registry;
    
    // Error handling
    bool has_pending_exception;
    v8::Global<v8::Value> pending_exception;
    
    // Performance
    struct {
        uint64_t execution_time_ms;
        uint64_t gc_time_ms;
        size_t heap_size;
        size_t heap_used;
        size_t object_count;
    } metrics;
    
    // Security
    bool allow_eval;
    bool allow_function_constructor;
    char *trusted_script_origins;
    size_t trusted_origin_count;
} js_runtime_t;

// JavaScript value wrapper
typedef struct js_value {
    v8::Global<v8::Value> value;
    js_runtime_t *runtime;
} js_value_t;

// JavaScript function wrapper
typedef struct js_function {
    v8::Global<v8::Function> function;
    js_runtime_t *runtime;
} js_function_t;

// JavaScript object wrapper
typedef struct js_object {
    v8::Global<v8::Object> object;
    js_runtime_t *runtime;
} js_object_t;

// JavaScript array wrapper
typedef struct js_array {
    v8::Global<v8::Array> array;
    js_runtime_t *runtime;
} js_array_t;

// JavaScript module
typedef struct js_module {
    char *name;
    char *source;
    v8::Global<v8::Module> module;
    bool is_evaluated;
} js_module_t;

// JavaScript promise
typedef struct js_promise {
    v8::Global<v8::Promise> promise;
    v8::Global<v8::Function> resolve;
    v8::Global<v8::Function> reject;
    js_runtime_t *runtime;
} js_promise_t;

// JavaScript error
typedef struct js_error {
    char *message;
    char *name;
    char *stack;
    int line_number;
    int column_number;
    char *file_name;
} js_error_t;

// Runtime management
js_runtime_t *js_runtime_create(void *event_loop);
void js_runtime_destroy(js_runtime_t *runtime);
void js_runtime_set_security_options(js_runtime_t *runtime, bool allow_eval, bool allow_function_constructor);
void js_runtime_add_trusted_origin(js_runtime_t *runtime, const char *origin);

// Context management
void js_runtime_enter_context(js_runtime_t *runtime);
void js_runtime_exit_context(js_runtime_t *runtime);
v8::Local<v8::Context> js_runtime_get_context(js_runtime_t *runtime);

// Script execution
js_value_t *js_runtime_execute(js_runtime_t *runtime, const char *script);
js_value_t *js_runtime_execute_file(js_runtime_t *runtime, const char *filename);
js_value_t *js_runtime_evaluate(js_runtime_t *runtime, const char *script, const char *filename);
bool js_runtime_has_exception(js_runtime_t *runtime);
js_error_t *js_runtime_get_exception(js_runtime_t *runtime);
void js_runtime_clear_exception(js_runtime_t *runtime);

// Module system
js_module_t *js_module_create(js_runtime_t *runtime, const char *name, const char *source);
js_value_t *js_module_evaluate(js_runtime_t *runtime, js_module_t *module);
js_value_t *js_runtime_import(js_runtime_t *runtime, const char *module_name);
void js_runtime_register_module(js_runtime_t *runtime, const char *name, const char *source);

// Value operations
js_value_t *js_value_create(js_runtime_t *runtime, v8::Local<v8::Value> value);
void js_value_destroy(js_value_t *value);
bool js_value_is_undefined(js_value_t *value);
bool js_value_is_null(js_value_t *value);
bool js_value_is_boolean(js_value_t *value);
bool js_value_is_number(js_value_t *value);
bool js_value_is_string(js_value_t *value);
bool js_value_is_object(js_value_t *value);
bool js_value_is_array(js_value_t *value);
bool js_value_is_function(js_value_t *value);
bool js_value_is_promise(js_value_t *value);

// Value conversion
bool js_value_to_bool(js_value_t *value);
double js_value_to_number(js_value_t *value);
char *js_value_to_string(js_value_t *value);
js_object_t *js_value_to_object(js_value_t *value);
js_array_t *js_value_to_array(js_value_t *value);
js_function_t *js_value_to_function(js_value_t *value);

// Value creation
js_value_t *js_value_create_undefined(js_runtime_t *runtime);
js_value_t *js_value_create_null(js_runtime_t *runtime);
js_value_t *js_value_create_bool(js_runtime_t *runtime, bool value);
js_value_t *js_value_create_number(js_runtime_t *runtime, double value);
js_value_t *js_value_create_string(js_runtime_t *runtime, const char *value);
js_value_t *js_value_create_string_utf8(js_runtime_t *runtime, const char *value, size_t length);
js_object_t *js_value_create_object(js_runtime_t *runtime);
js_array_t *js_value_create_array(js_runtime_t *runtime, size_t length);
js_function_t *js_value_create_function(js_runtime_t *runtime, v8::FunctionCallback callback);
js_promise_t *js_value_create_promise(js_runtime_t *runtime);

// Object operations
js_object_t *js_object_create(js_runtime_t *runtime);
js_object_t *js_object_create_with_prototype(js_runtime_t *runtime, js_object_t *prototype);
void js_object_destroy(js_object_t *object);
js_value_t *js_object_get(js_object_t *object, const char *key);
void js_object_set(js_object_t *object, const char *key, js_value_t *value);
bool js_object_has(js_object_t *object, const char *key);
void js_object_delete(js_object_t *object, const char *key);
char **js_object_get_keys(js_object_t *object, size_t *count);
size_t js_object_get_length(js_object_t *object);

// Array operations
js_array_t *js_array_create(js_runtime_t *runtime, size_t length);
void js_array_destroy(js_array_t *array);
js_value_t *js_array_get(js_array_t *array, size_t index);
void js_array_set(js_array_t *array, size_t index, js_value_t *value);
void js_array_push(js_array_t *array, js_value_t *value);
js_value_t *js_array_pop(js_array_t *array);
size_t js_array_get_length(js_array_t *array);
void js_array_set_length(js_array_t *array, size_t length);

// Function operations
js_function_t *js_function_create(js_runtime_t *runtime, v8::FunctionCallback callback);
void js_function_destroy(js_function_t *function);
js_value_t *js_function_call(js_function_t *function, js_object_t *this_obj, size_t argc, js_value_t **argv);
js_value_t *js_function_call_with_args(js_function_t *function, js_object_t *this_obj, ...);

// Promise operations
js_promise_t *js_promise_create(js_runtime_t *runtime);
void js_promise_destroy(js_promise_t *promise);
void js_promise_resolve(js_promise_t *promise, js_value_t *value);
void js_promise_reject(js_promise_t *promise, js_value_t *value);
js_value_t *js_promise_then(js_promise_t *promise, js_function_t *on_fulfilled, js_function_t *on_rejected);
js_value_t *js_promise_catch(js_promise_t *promise, js_function_t *on_rejected);

// DOM bindings
void js_bind_dom(js_runtime_t *runtime, dom_document_t *document);
void js_bind_element(js_runtime_t *runtime, dom_element_t *element);
void js_bind_node(js_runtime_t *runtime, dom_node_t *node);
js_object_t *js_wrap_dom_node(js_runtime_t *runtime, dom_node_t *node);
dom_node_t *js_unwrap_dom_node(js_value_t *value);

// CSS bindings
void js_bind_css(js_runtime_t *runtime, css_stylesheet_t *stylesheet);
js_object_t *js_wrap_css_style(js_runtime_t *runtime, css_computed_style_t *style);
css_computed_style_t *js_unwrap_css_style(js_value_t *value);

// Event bindings
void js_bind_events(js_runtime_t *runtime);
js_function_t *js_create_event_handler(js_runtime_t *runtime, dom_event_type_t type, js_function_t *handler);
void js_dispatch_dom_event(js_runtime_t *runtime, dom_event_t *event);

// Console API
void js_bind_console(js_runtime_t *runtime);
void js_console_log(js_runtime_t *runtime, const char *message);
void js_console_error(js_runtime_t *runtime, const char *message);
void js_console_warn(js_runtime_t *runtime, const char *message);
void js_console_info(js_runtime_t *runtime, const char *message);

// Timer API
void js_bind_timers(js_runtime_t *runtime);
uint32_t js_set_timeout(js_runtime_t *runtime, js_function_t *callback, uint32_t delay_ms);
uint32_t js_set_interval(js_runtime_t *runtime, js_function_t *callback, uint32_t interval_ms);
void js_clear_timeout(js_runtime_t *runtime, uint32_t timer_id);
void js_clear_interval(js_runtime_t *runtime, uint32_t timer_id);

// Animation Frame API
void js_bind_animation_frame(js_runtime_t *runtime);
uint32_t js_request_animation_frame(js_runtime_t *runtime, js_function_t *callback);
void js_cancel_animation_frame(js_runtime_t *runtime, uint32_t frame_id);

// Fetch API
void js_bind_fetch(js_runtime_t *runtime);
js_promise_t *js_fetch(js_runtime_t *runtime, const char *url, js_object_t *options);

// WebSocket API
void js_bind_websocket(js_runtime_t *runtime);
js_object_t *js_websocket_create(js_runtime_t *runtime, const char *url);
void js_websocket_send(js_runtime_t *runtime, js_object_t *websocket, const char *data);
void js_websocket_close(js_runtime_t *runtime, js_object_t *websocket);

// Canvas API
void js_bind_canvas(js_runtime_t *runtime);
js_object_t *js_canvas_get_context_2d(js_runtime_t *runtime, js_object_t *canvas);
void js_canvas_fill_rect(js_runtime_t *runtime, js_object_t *ctx, double x, double y, double width, double height);
void js_canvas_stroke_rect(js_runtime_t *runtime, js_object_t *ctx, double x, double y, double width, double height);
void js_canvas_draw_image(js_runtime_t *runtime, js_object_t *ctx, js_object_t *image, double dx, double dy);

// Local Storage API
void js_bind_local_storage(js_runtime_t *runtime);
char *js_local_storage_get_item(js_runtime_t *runtime, const char *key);
void js_local_storage_set_item(js_runtime_t *runtime, const char *key, const char *value);
void js_local_storage_remove_item(js_runtime_t *runtime, const char *key);
void js_local_storage_clear(js_runtime_t *runtime);
char **js_local_storage_get_keys(js_runtime_t *runtime, size_t *count);

// Performance API
void js_bind_performance(js_runtime_t *runtime);
uint64_t js_performance_now(js_runtime_t *runtime);
void js_performance_mark(js_runtime_t *runtime, const char *name);
void js_performance_measure(js_runtime_t *runtime, const char *name, const char *start_mark, const char *end_mark);

// Security
bool js_is_script_trusted(js_runtime_t *runtime, const char *origin);
bool js_validate_script_content(js_runtime_t *runtime, const char *script);
void js_sanitize_script(js_runtime_t *runtime, char *script);

// Error handling
js_error_t *js_error_create(const char *message, const char *name, const char *stack);
void js_error_destroy(js_error_t *error);
char *js_error_to_string(js_error_t *error);

// Garbage collection
void js_gc_collect(js_runtime_t *runtime);
void js_gc_set_memory_limit(js_runtime_t *runtime, size_t limit_bytes);
size_t js_gc_get_memory_usage(js_runtime_t *runtime);
void js_gc_get_statistics(js_runtime_t *runtime, size_t *used, size_t *total);

// Debugging
void js_enable_debugging(js_runtime_t *runtime, bool enabled);
void js_set_debug_port(js_runtime_t *runtime, int port);
void js_inspect_object(js_runtime_t *runtime, js_object_t *object);
void js_inspect_heap(js_runtime_t *runtime);

// Profiling
void js_start_profiling(js_runtime_t *runtime);
void js_stop_profiling(js_runtime_t *runtime);
void js_start_cpu_profiling(js_runtime_t *runtime);
void js_stop_cpu_profiling(js_runtime_t *runtime);

// Module system extensions
void js_register_native_module(js_runtime_t *runtime, const char *name, void (*init_func)(js_runtime_t *));
void js_require_native_module(js_runtime_t *runtime, const char *name);

// WebAssembly support
void js_bind_webassembly(js_runtime_t *runtime);
js_object_t *js_webassembly_compile(js_runtime_t *runtime, const char *bytecode);
js_object_t *js_webassembly_instantiate(js_runtime_t *runtime, js_object_t *module, js_object_t *import_object);

// Worker threads
typedef struct js_worker js_worker_t;
js_worker_t *js_worker_create(js_runtime_t *runtime, const char *script_url);
void js_worker_destroy(js_worker_t *worker);
void js_worker_post_message(js_worker_t *worker, js_value_t *message);
js_value_t *js_worker_get_onmessage(js_worker_t *worker);
void js_worker_set_onmessage(js_worker_t *worker, js_function_t *handler);

// SharedArrayBuffer and Atomics
js_object_t *js_shared_array_buffer_create(js_runtime_t *runtime, size_t byte_length);
void *js_shared_array_buffer_get_data(js_runtime_t *runtime, js_object_t *sab);
js_value_t *js_atomics_add(js_runtime_t *runtime, js_object_t *typed_array, size_t index, int32_t value);
js_value_t *js_atomics_sub(js_runtime_t *runtime, js_object_t *typed_array, size_t index, int32_t value);
js_value_t *js_atomics_and(js_runtime_t *runtime, js_object_t *typed_array, size_t index, int32_t value);
js_value_t *js_atomics_or(js_runtime_t *runtime, js_object_t *typed_array, size_t index, int32_t value);
js_value_t *js_atomics_xor(js_runtime_t *runtime, js_object_t *typed_array, size_t index, int32_t value);
js_value_t *js_atomics_compare_exchange(js_runtime_t *runtime, js_object_t *typed_array, size_t index, int32_t expected_value, int32_t new_value);

// BigInt support
js_value_t *js_bigint_create(js_runtime_t *runtime, int64_t value);
js_value_t *js_bigint_create_string(js_runtime_t *runtime, const char *str);
char *js_bigint_to_string(js_runtime_t *runtime, js_value_t *bigint);
int64_t js_bigint_to_int64(js_runtime_t *runtime, js_value_t *bigint);

// Proxy and Reflect
js_object_t *js_proxy_create(js_runtime_t *runtime, js_object_t *target, js_object_t *handler);
js_object_t *js_reflect_get(js_runtime_t *runtime, js_object_t *target, js_value_t *property_key);
js_object_t *js_reflect_set(js_runtime_t *runtime, js_object_t *target, js_value_t *property_key, js_value_t *value);
bool js_reflect_has(js_runtime_t *runtime, js_object_t *target, js_value_t *property_key);
bool js_reflect_delete_property(js_runtime_t *runtime, js_object_t *target, js_value_t *property_key);

#endif // JS_JS_H
