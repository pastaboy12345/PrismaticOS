#include <dom/dom.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// DOM node implementation
dom_node_t *dom_node_create(dom_node_type_t type) {
    dom_node_t *node = malloc(sizeof(dom_node_t));
    if (!node) return NULL;
    
    memset(node, 0, sizeof(dom_node_t));
    node->type = type;
    node->ref_count = 1;
    
    return node;
}

void dom_node_destroy(dom_node_t *node) {
    if (!node) return;
    
    // Free attributes
    for (size_t i = 0; i < node->attr_count; i++) {
        free(node->attr_names[i]);
        free(node->attr_values[i]);
    }
    free(node->attr_names);
    free(node->attr_values);
    
    // Free class names
    for (size_t i = 0; i < node->class_count; i++) {
        free(node->class_names[i]);
    }
    free(node->class_names);
    
    // Free other strings
    free(node->tag_name);
    free(node->id);
    free(node->text_content);
    
    // Free event listeners
    free(node->event_listeners);
    
    // Remove from parent
    if (node->parent) {
        if (node->parent->first_child == node) {
            node->parent->first_child = node->next_sibling;
        }
        if (node->parent->last_child == node) {
            node->parent->last_child = node->prev_sibling;
        }
        if (node->prev_sibling) {
            node->prev_sibling->next_sibling = node->next_sibling;
        }
        if (node->next_sibling) {
            node->next_sibling->prev_sibling = node->prev_sibling;
        }
    }
    
    // Recursively destroy children
    dom_node_t *child = node->first_child;
    while (child) {
        dom_node_t *next = child->next_sibling;
        dom_node_destroy(child);
        child = next;
    }
    
    free(node);
}

dom_node_t *dom_node_ref(dom_node_t *node) {
    if (node) {
        node->ref_count++;
    }
    return node;
}

void dom_node_unref(dom_node_t *node) {
    if (node && --node->ref_count == 0) {
        dom_node_destroy(node);
    }
}

// DOM document implementation
dom_document_t *dom_document_create(const char *url) {
    dom_document_t *doc = malloc(sizeof(dom_document_t));
    if (!doc) return NULL;
    
    memset(doc, 0, sizeof(dom_document_t));
    doc->base.type = DOM_NODE_TYPE_DOCUMENT;
    doc->base.ref_count = 1;
    
    if (url) {
        doc->url = strdup(url);
    }
    
    return doc;
}

void dom_document_destroy(dom_document_t *doc) {
    if (!doc) return;
    
    free(doc->url);
    free(doc->title);
    free(doc->global_listeners);
    
    // Base class cleanup
    dom_node_destroy(&doc->base);
}

// DOM element implementation
dom_element_t *dom_element_create(const char *tag_name) {
    dom_element_t *element = malloc(sizeof(dom_element_t));
    if (!element) return NULL;
    
    memset(element, 0, sizeof(dom_element_t));
    element->base.type = DOM_NODE_TYPE_ELEMENT;
    element->base.ref_count = 1;
    
    if (tag_name) {
        element->base.tag_name = strdup(tag_name);
    }
    
    return element;
}

// DOM text node implementation
dom_text_t *dom_text_create(const char *data) {
    dom_text_t *text = malloc(sizeof(dom_text_t));
    if (!text) return NULL;
    
    memset(text, 0, sizeof(dom_text_t));
    text->base.type = DOM_NODE_TYPE_TEXT;
    text->base.ref_count = 1;
    
    if (data) {
        text->data = strdup(data);
        text->base.text_content = strdup(data);
    }
    
    return text;
}

// DOM tree manipulation
void dom_node_append_child(dom_node_t *parent, dom_node_t *child) {
    if (!parent || !child) return;
    
    // Remove child from current parent if any
    if (child->parent) {
        dom_node_remove_child(child->parent, child);
    }
    
    // Add to new parent
    child->parent = parent;
    
    if (!parent->first_child) {
        parent->first_child = child;
        parent->last_child = child;
    } else {
        parent->last_child->next_sibling = child;
        child->prev_sibling = parent->last_child;
        parent->last_child = child;
    }
}

void dom_node_remove_child(dom_node_t *parent, dom_node_t *child) {
    if (!parent || !child || child->parent != parent) return;
    
    // Remove from linked list
    if (parent->first_child == child) {
        parent->first_child = child->next_sibling;
    }
    if (parent->last_child == child) {
        parent->last_child = child->prev_sibling;
    }
    if (child->prev_sibling) {
        child->prev_sibling->next_sibling = child->next_sibling;
    }
    if (child->next_sibling) {
        child->next_sibling->prev_sibling = child->prev_sibling;
    }
    
    // Clear parent pointers
    child->parent = NULL;
    child->prev_sibling = NULL;
    child->next_sibling = NULL;
}

// Attribute management
void dom_node_set_attribute(dom_node_t *node, const char *name, const char *value) {
    if (!node || !name) return;
    
    // Check if attribute already exists
    for (size_t i = 0; i < node->attr_count; i++) {
        if (strcmp(node->attr_names[i], name) == 0) {
            free(node->attr_values[i]);
            node->attr_values[i] = value ? strdup(value) : NULL;
            return;
        }
    }
    
    // Add new attribute
    node->attr_names = realloc(node->attr_names, (node->attr_count + 1) * sizeof(char*));
    node->attr_values = realloc(node->attr_values, (node->attr_count + 1) * sizeof(char*));
    
    node->attr_names[node->attr_count] = strdup(name);
    node->attr_values[node->attr_count] = value ? strdup(value) : NULL;
    node->attr_count++;
}

const char *dom_node_get_attribute(dom_node_t *node, const char *name) {
    if (!node || !name) return NULL;
    
    for (size_t i = 0; i < node->attr_count; i++) {
        if (strcmp(node->attr_names[i], name) == 0) {
            return node->attr_values[i];
        }
    }
    
    return NULL;
}

// Class management
void dom_node_add_class(dom_node_t *node, const char *class_name) {
    if (!node || !class_name) return;
    
    // Check if class already exists
    for (size_t i = 0; i < node->class_count; i++) {
        if (strcmp(node->class_names[i], class_name) == 0) {
            return;
        }
    }
    
    // Add new class
    node->class_names = realloc(node->class_names, (node->class_count + 1) * sizeof(char*));
    node->class_names[node->class_count] = strdup(class_name);
    node->class_count++;
}

bool dom_node_has_class(dom_node_t *node, const char *class_name) {
    if (!node || !class_name) return false;
    
    for (size_t i = 0; i < node->class_count; i++) {
        if (strcmp(node->class_names[i], class_name) == 0) {
            return true;
        }
    }
    
    return false;
}

// Text content
void dom_node_set_text_content(dom_node_t *node, const char *text) {
    if (!node) return;
    
    free(node->text_content);
    node->text_content = text ? strdup(text) : NULL;
    
    // For text nodes, also update data
    if (node->type == DOM_NODE_TYPE_TEXT) {
        dom_text_t *text_node = (dom_text_t*)node;
        free(text_node->data);
        text_node->data = text ? strdup(text) : NULL;
    }
}

const char *dom_node_get_text_content(dom_node_t *node) {
    if (!node) return NULL;
    
    if (node->type == DOM_NODE_TYPE_TEXT) {
        dom_text_t *text_node = (dom_text_t*)node;
        return text_node->data;
    }
    
    return node->text_content;
}

// Event handling
void dom_add_event_listener(dom_node_t *node, dom_event_type_t type, 
                           void (*handler)(dom_event_t *event, void *user_data), 
                           void *user_data) {
    if (!node || !handler) return;
    
    dom_event_listener_t *listener = malloc(sizeof(dom_event_listener_t));
    listener->type = type;
    listener->handler = handler;
    listener->user_data = user_data;
    listener->capture = false;
    listener->passive = false;
    listener->once = false;
    
    node->event_listeners = realloc(node->event_listeners, 
                                  (node->event_listener_count + 1) * sizeof(dom_event_listener_t*));
    node->event_listeners[node->event_listener_count] = listener;
    node->event_listener_count++;
}

void dom_dispatch_event(dom_node_t *target, dom_event_t *event) {
    if (!target || !event) return;
    
    event->target = target;
    event->current_target = target;
    event->timestamp = 0;  // TODO: get current time
    
    // Call event listeners
    for (size_t i = 0; i < target->event_listener_count; i++) {
        dom_event_listener_t *listener = target->event_listeners[i];
        if (listener->type == event->type) {
            listener->handler(event, listener->user_data);
        }
    }
}

// Event creation
dom_event_t *dom_event_create(dom_event_type_t type) {
    dom_event_t *event = malloc(sizeof(dom_event_t));
    if (!event) return NULL;
    
    memset(event, 0, sizeof(dom_event_t));
    event->type = type;
    event->bubbles = true;
    event->cancelable = true;
    
    return event;
}

void dom_event_destroy(dom_event_t *event) {
    if (!event) return;
    
    // Free event-specific data
    if (event->type == DOM_EVENT_CHANGE) {
        free(event->data.change.old_value);
        free(event->data.change.new_value);
    }
    
    free(event);
}

// Utility functions
bool dom_node_is_element(dom_node_t *node) {
    return node && node->type == DOM_NODE_TYPE_ELEMENT;
}

bool dom_node_is_text(dom_node_t *node) {
    return node && node->type == DOM_NODE_TYPE_TEXT;
}

bool dom_node_is_document(dom_node_t *node) {
    return node && node->type == DOM_NODE_TYPE_DOCUMENT;
}

// Element by ID search
dom_node_t *dom_document_get_element_by_id(dom_document_t *doc, const char *id) {
    if (!doc || !id) return NULL;
    
    return dom_node_get_element_by_id_recursive(doc->base.first_child, id);
}

static dom_node_t *dom_node_get_element_by_id_recursive(dom_node_t *node, const char *id) {
    if (!node || !id) return NULL;
    
    // Check current node
    if (dom_node_is_element(node) && node->id && strcmp(node->id, id) == 0) {
        return node;
    }
    
    // Recursively check children
    for (dom_node_t *child = node->first_child; child; child = child->next_sibling) {
        dom_node_t *result = dom_node_get_element_by_id_recursive(child, id);
        if (result) return result;
    }
    
    return NULL;
}
