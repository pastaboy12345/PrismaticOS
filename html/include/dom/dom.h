#ifndef DOM_DOM_H
#define DOM_DOM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// DOM node types
typedef enum {
    DOM_NODE_TYPE_ELEMENT,
    DOM_NODE_TYPE_TEXT,
    DOM_NODE_TYPE_COMMENT,
    DOM_NODE_TYPE_DOCUMENT,
    DOM_NODE_TYPE_DOCUMENT_FRAGMENT
} dom_node_type_t;

// DOM event types
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
    DOM_EVENT_RESIZE,
    DOM_EVENT_CHANGE,
    DOM_EVENT_SUBMIT
} dom_event_type_t;

// Forward declarations
typedef struct dom_node dom_node_t;
typedef struct dom_document dom_document_t;
typedef struct dom_element dom_element_t;
typedef struct dom_text dom_text_t;
typedef struct dom_event dom_event_t;
typedef struct dom_event_listener dom_event_listener_t;

// DOM node structure
struct dom_node {
    dom_node_type_t type;
    char *tag_name;
    char *id;
    char **class_names;
    size_t class_count;
    
    // Attributes
    char **attr_names;
    char **attr_values;
    size_t attr_count;
    
    // Content
    char *text_content;
    
    // Hierarchy
    dom_node_t *parent;
    dom_node_t *first_child;
    dom_node_t *last_child;
    dom_node_t *prev_sibling;
    dom_node_t *next_sibling;
    
    // Event listeners
    dom_event_listener_t **event_listeners;
    size_t event_listener_count;
    
    // Computed style
    void *computed_style;
    
    // Layout information
    void *layout_box;
    
    // Reference counting
    uint32_t ref_count;
};

// DOM document
struct dom_document {
    dom_node_t base;  // Base node
    dom_node_t *document_element;
    char *url;
    char *title;
    
    // Ready state
    bool ready;
    bool loading;
    
    // Event handling
    dom_event_listener_t **global_listeners;
    size_t global_listener_count;
};

// DOM element
struct dom_element {
    dom_node_t base;  // Base node
    
    // Element-specific properties
    char *namespace;
    bool is_void;
    
    // Style
    char *style;
    char *class_attr;
};

// DOM text node
struct dom_text {
    dom_node_t base;  // Base node
    char *data;
};

// DOM event
struct dom_event {
    dom_event_type_t type;
    dom_node_t *target;
    dom_node_t *current_target;
    uint64_t timestamp;
    bool bubbles;
    bool cancelable;
    bool default_prevented;
    bool propagation_stopped;
    
    // Event-specific data
    union {
        struct {
            int x, y;
            int button;
            int buttons;
            int client_x, client_y;
            int screen_x, screen_y;
        } mouse;
        struct {
            uint32_t keycode;
            uint32_t char_code;
            bool ctrl_key;
            bool shift_key;
            bool alt_key;
            bool meta_key;
        } key;
        struct {
            char *old_value;
            char *new_value;
        } change;
    } data;
};

// Event listener
struct dom_event_listener {
    dom_event_type_t type;
    void (*handler)(dom_event_t *event, void *user_data);
    void *user_data;
    bool capture;
    bool passive;
    bool once;
};

// DOM node creation and management
dom_node_t *dom_node_create(dom_node_type_t type);
void dom_node_destroy(dom_node_t *node);
dom_node_t *dom_node_ref(dom_node_t *node);
void dom_node_unref(dom_node_t *node);

// DOM document creation
dom_document_t *dom_document_create(const char *url);
void dom_document_destroy(dom_document_t *doc);

// DOM element creation
dom_element_t *dom_element_create(const char *tag_name);
dom_node_t *dom_element_create_with_namespace(const char *tag_name, const char *namespace);

// DOM text node creation
dom_text_t *dom_text_create(const char *data);

// DOM tree manipulation
void dom_node_append_child(dom_node_t *parent, dom_node_t *child);
void dom_node_prepend_child(dom_node_t *parent, dom_node_t *child);
void dom_node_insert_before(dom_node_t *parent, dom_node_t *new_child, dom_node_t *reference_child);
void dom_node_insert_after(dom_node_t *parent, dom_node_t *new_child, dom_node_t *reference_child);
void dom_node_remove_child(dom_node_t *parent, dom_node_t *child);
void dom_node_replace_child(dom_node_t *parent, dom_node_t *new_child, dom_node_t *old_child);

// DOM attribute management
void dom_node_set_attribute(dom_node_t *node, const char *name, const char *value);
const char *dom_node_get_attribute(dom_node_t *node, const char *name);
void dom_node_remove_attribute(dom_node_t *node, const char *name);
bool dom_node_has_attribute(dom_node_t *node, const char *name);

// DOM class management
void dom_node_add_class(dom_node_t *node, const char *class_name);
void dom_node_remove_class(dom_node_t *node, const char *class_name);
bool dom_node_has_class(dom_node_t *node, const char *class_name);
void dom_node_toggle_class(dom_node_t *node, const char *class_name);

// DOM text content
void dom_node_set_text_content(dom_node_t *node, const char *text);
const char *dom_node_get_text_content(dom_node_t *node);
void dom_node_append_text(dom_node_t *node, const char *text);

// DOM querying
dom_node_t *dom_document_get_element_by_id(dom_document_t *doc, const char *id);
dom_node_t **dom_document_get_elements_by_tag_name(dom_document_t *doc, const char *tag_name, size_t *count);
dom_node_t **dom_document_get_elements_by_class_name(dom_document_t *doc, const char *class_name, size_t *count);
dom_node_t **dom_document_query_selector_all(dom_document_t *doc, const char *selector, size_t *count);
dom_node_t *dom_document_query_selector(dom_document_t *doc, const char *selector);

// DOM traversal
dom_node_t *dom_node_get_first_element_child(dom_node_t *node);
dom_node_t *dom_node_get_last_element_child(dom_node_t *node);
dom_node_t *dom_node_get_next_element_sibling(dom_node_t *node);
dom_node_t *dom_node_get_previous_element_sibling(dom_node_t *node);
dom_node_t *dom_node_get_parent_element(dom_node_t *node);

// Event handling
void dom_add_event_listener(dom_node_t *node, dom_event_type_t type, 
                           void (*handler)(dom_event_t *event, void *user_data), 
                           void *user_data);
void dom_remove_event_listener(dom_node_t *node, dom_event_type_t type, 
                              void (*handler)(dom_event_t *event, void *user_data));
void dom_dispatch_event(dom_node_t *target, dom_event_t *event);
dom_event_t *dom_event_create(dom_event_type_t type);
void dom_event_destroy(dom_event_t *event);

// Event utility functions
bool dom_event_matches_selector(dom_event_t *event, const char *selector);
void dom_event_stop_propagation(dom_event_t *event);
void dom_event_prevent_default(dom_event_t *event);

// DOM mutation observers (simplified)
typedef struct dom_mutation_observer dom_mutation_observer_t;
typedef void (*dom_mutation_callback_t)(dom_mutation_record_t **mutations, size_t count, dom_mutation_observer_t *observer);

dom_mutation_observer_t *dom_mutation_observer_create(dom_mutation_callback_t callback);
void dom_mutation_observer_observe(dom_mutation_observer_t *observer, dom_node_t *target, uint32_t options);
void dom_mutation_observer_disconnect(dom_mutation_observer_t *observer);

// Utility functions
bool dom_node_is_element(dom_node_t *node);
bool dom_node_is_text(dom_node_t *node);
bool dom_node_is_document(dom_node_t *node);
const char *dom_node_get_local_name(dom_node_t *node);
const char *dom_node_get_namespace(dom_node_t *node);
bool dom_node_matches(dom_node_t *node, const char *selector);
bool dom_node_contains(dom_node_t *parent, dom_node_t *child);

// Constants
#define DOM_BUBBLING_PHASE 1
#define DOM_AT_TARGET_PHASE 2
#define DOM_CAPTURING_PHASE 3

#endif // DOM_DOM_H
