#include <html/html.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// HTML parser implementation
html_parser_t *html_parser_create(void) {
    html_parser_t *parser = malloc(sizeof(html_parser_t));
    if (!parser) return NULL;
    
    memset(parser, 0, sizeof(html_parser_t));
    
    return parser;
}

void html_parser_destroy(html_parser_t *parser) {
    if (!parser) return;
    
    // Clean up current token
    if (parser->token_ready) {
        free(parser->current_token.tag_name);
        free(parser->current_token.data);
        for (size_t i = 0; i < parser->current_token.attr_count; i++) {
            free(parser->current_token.attr_names[i]);
            free(parser->current_token.attr_values[i]);
        }
        free(parser->current_token.attr_names);
        free(parser->current_token.attr_values);
    }
    
    free(parser);
}

// Token parsing functions
static char *html_parse_tag_name(html_parser_t *parser) {
    const char *input = parser->input + parser->position;
    size_t start = parser->position;
    
    // Parse tag name (letters, digits, hyphens)
    while (parser->position < parser->length) {
        char c = parser->input[parser->position];
        if (!isalnum(c) && c != '-' && c != '_') {
            break;
        }
        parser->position++;
    }
    
    if (start == parser->position) {
        return NULL;
    }
    
    size_t length = parser->position - start;
    char *tag_name = malloc(length + 1);
    memcpy(tag_name, input, length);
    tag_name[length] = '\0';
    
    return tag_name;
}

static char *html_parse_attribute_value(html_parser_t *parser) {
    // Skip whitespace
    while (parser->position < parser->length && isspace(parser->input[parser->position])) {
        parser->position++;
    }
    
    if (parser->position >= parser->length) {
        return NULL;
    }
    
    char quote = parser->input[parser->position];
    if (quote == '"' || quote == '\'') {
        // Quoted value
        parser->position++;
        const char *start = parser->input + parser->position;
        
        while (parser->position < parser->length && parser->input[parser->position] != quote) {
            parser->position++;
        }
        
        if (parser->position >= parser->length) {
            return NULL;  // Unterminated string
        }
        
        size_t length = parser->position - (start - parser->input);
        char *value = malloc(length + 1);
        memcpy(value, start, length);
        value[length] = '\0';
        
        parser->position++;  // Skip closing quote
        return value;
    } else {
        // Unquoted value
        const char *start = parser->input + parser->position;
        
        while (parser->position < parser->length && 
               !isspace(parser->input[parser->position]) &&
               parser->input[parser->position] != '>' &&
               parser->input[parser->position] != '/') {
            parser->position++;
        }
        
        size_t length = parser->position - (start - parser->input);
        char *value = malloc(length + 1);
        memcpy(value, start, length);
        value[length] = '\0';
        
        return value;
    }
}

static void html_parse_attributes(html_parser_t *parser, html_token_t *token) {
    while (parser->position < parser->length) {
        // Skip whitespace
        while (parser->position < parser->length && isspace(parser->input[parser->position])) {
            parser->position++;
        }
        
        if (parser->position >= parser->length) break;
        
        char c = parser->input[parser->position];
        if (c == '>' || c == '/') {
            break;
        }
        
        // Parse attribute name
        size_t name_start = parser->position;
        while (parser->position < parser->length && 
               !isspace(parser->input[parser->position]) &&
               parser->input[parser->position] != '=' &&
               parser->input[parser->position] != '>' &&
               parser->input[parser->position] != '/') {
            parser->position++;
        }
        
        if (name_start == parser->position) break;
        
        size_t name_length = parser->position - name_start;
        char *name = malloc(name_length + 1);
        memcpy(name, parser->input + name_start, name_length);
        name[name_length] = '\0';
        
        // Skip whitespace
        while (parser->position < parser->length && isspace(parser->input[parser->position])) {
            parser->position++;
        }
        
        char *value = NULL;
        if (parser->position < parser->length && parser->input[parser->position] == '=') {
            parser->position++;
            value = html_parse_attribute_value(parser);
        }
        
        // Add attribute to token
        token->attr_names = realloc(token->attr_names, (token->attr_count + 1) * sizeof(char*));
        token->attr_values = realloc(token->attr_values, (token->attr_count + 1) * sizeof(char*));
        token->attr_names[token->attr_count] = name;
        token->attr_values[token->attr_count] = value;
        token->attr_count++;
    }
}

static html_token_t *html_parse_tag(html_parser_t *parser) {
    html_token_t *token = malloc(sizeof(html_token_t));
    memset(token, 0, sizeof(html_token_t));
    
    parser->position++;  // Skip '<'
    
    // Check for end tag
    if (parser->position < parser->length && parser->input[parser->position] == '/') {
        token->type = HTML_TOKEN_END_TAG;
        parser->position++;
    } else {
        token->type = HTML_TOKEN_START_TAG;
    }
    
    // Parse tag name
    token->tag_name = html_parse_tag_name(parser);
    if (!token->tag_name) {
        free(token);
        return NULL;
    }
    
    // Parse attributes
    html_parse_attributes(parser, token);
    
    // Check for self-closing
    if (parser->position < parser->length && parser->input[parser->position] == '/') {
        token->self_closing = true;
        parser->position++;
    }
    
    // Expect '>'
    if (parser->position < parser->length && parser->input[parser->position] == '>') {
        parser->position++;
    } else {
        // Malformed tag
        free(token->tag_name);
        for (size_t i = 0; i < token->attr_count; i++) {
            free(token->attr_names[i]);
            free(token->attr_values[i]);
        }
        free(token->attr_names);
        free(token->attr_values);
        free(token);
        return NULL;
    }
    
    return token;
}

static html_token_t *html_parse_text(html_parser_t *parser) {
    const char *start = parser->input + parser->position;
    
    while (parser->position < parser->length && parser->input[parser->position] != '<') {
        parser->position++;
    }
    
    if (start == parser->input + parser->position) {
        return NULL;  // No text content
    }
    
    size_t length = parser->position - (start - parser->input);
    char *text = malloc(length + 1);
    memcpy(text, start, length);
    text[length] = '\0';
    
    html_token_t *token = malloc(sizeof(html_token_t));
    memset(token, 0, sizeof(html_token_t));
    token->type = HTML_TOKEN_TEXT;
    token->data = text;
    
    return token;
}

static html_token_t *html_parse_comment(html_parser_t *parser) {
    parser->position += 4;  // Skip '<!--'
    
    const char *start = parser->input + parser->position;
    
    // Look for '-->'
    while (parser->position < parser->length - 2) {
        if (parser->input[parser->position] == '-' &&
            parser->input[parser->position + 1] == '-' &&
            parser->input[parser->position + 2] == '>') {
            break;
        }
        parser->position++;
    }
    
    if (parser->position >= parser->length - 2) {
        return NULL;  // Unterminated comment
    }
    
    size_t length = parser->position - (start - parser->input);
    char *comment = malloc(length + 1);
    memcpy(comment, start, length);
    comment[length] = '\0';
    
    html_token_t *token = malloc(sizeof(html_token_t));
    memset(token, 0, sizeof(html_token_t));
    token->type = HTML_TOKEN_COMMENT;
    token->data = comment;
    
    parser->position += 3;  // Skip '-->'
    
    return token;
}

html_token_t *html_parser_next_token(html_parser_t *parser) {
    if (!parser || parser->position >= parser->length) {
        return NULL;
    }
    
    // Skip whitespace between tags
    while (parser->position < parser->length && isspace(parser->input[parser->position])) {
        parser->position++;
    }
    
    if (parser->position >= parser->length) {
        return NULL;
    }
    
    if (parser->input[parser->position] == '<') {
        // Check for comment
        if (parser->position + 3 < parser->length &&
            parser->input[parser->position + 1] == '!' &&
            parser->input[parser->position + 2] == '-' &&
            parser->input[parser->position + 3] == '-') {
            return html_parse_comment(parser);
        }
        
        // Check for DOCTYPE
        if (parser->position + 8 < parser->length &&
            strncasecmp(parser->input + parser->position, "<!DOCTYPE", 9) == 0) {
            // Skip DOCTYPE for now
            while (parser->position < parser->length && parser->input[parser->position] != '>') {
                parser->position++;
            }
            if (parser->position < parser->length) {
                parser->position++;
            }
            
            html_token_t *token = malloc(sizeof(html_token_t));
            memset(token, 0, sizeof(html_token_t));
            token->type = HTML_TOKEN_DOCTYPE;
            return token;
        }
        
        return html_parse_tag(parser);
    } else {
        return html_parse_text(parser);
    }
}

// DOM building functions
static dom_node_t *html_build_dom_tree(html_parser_t *parser) {
    html_document_t *doc = html_document_create("about:blank");
    if (!doc) return NULL;
    
    parser->document = &doc->base;
    parser->current_node = &doc->base;
    
    html_token_t *token;
    while ((token = html_parser_next_token(parser)) != NULL) {
        switch (token->type) {
            case HTML_TOKEN_START_TAG:
                html_handle_start_tag(parser, token);
                break;
                
            case HTML_TOKEN_END_TAG:
                html_handle_end_tag(parser, token);
                break;
                
            case HTML_TOKEN_TEXT:
                html_handle_text(parser, token);
                break;
                
            case HTML_TOKEN_COMMENT:
                // Ignore comments for now
                break;
                
            case HTML_TOKEN_DOCTYPE:
                // Handle DOCTYPE
                break;
                
            default:
                break;
        }
        
        // Clean up token
        free(token->tag_name);
        free(token->data);
        for (size_t i = 0; i < token->attr_count; i++) {
            free(token->attr_names[i]);
            free(token->attr_values[i]);
        }
        free(token->attr_names);
        free(token->attr_values);
        free(token);
    }
    
    return &doc->base;
}

static void html_handle_start_tag(html_parser_t *parser, html_token_t *token) {
    dom_element_t *element = dom_element_create(token->tag_name);
    if (!element) return;
    
    // Set attributes
    for (size_t i = 0; i < token->attr_count; i++) {
        dom_node_set_attribute(&element->base, token->attr_names[i], token->attr_values[i]);
    }
    
    // Set ID
    const char *id = dom_node_get_attribute(&element->base, "id");
    if (id) {
        element->base.id = strdup(id);
    }
    
    // Set class
    const char *class_attr = dom_node_get_attribute(&element->base, "class");
    if (class_attr) {
        // Parse space-separated class names
        char *class_copy = strdup(class_attr);
        char *token = strtok(class_copy, " ");
        while (token) {
            dom_node_add_class(&element->base, token);
            token = strtok(NULL, " ");
        }
        free(class_copy);
    }
    
    // Add to DOM
    dom_node_append_child(parser->current_node, &element->base);
    
    // Handle special elements
    if (strcasecmp(token->tag_name, "html") == 0) {
        html_document_t *doc = (html_document_t*)parser->document;
        doc->html_element = element;
        parser->in_body = false;
    } else if (strcasecmp(token->tag_name, "head") == 0) {
        html_document_t *doc = (html_document_t*)parser->document;
        doc->head_element = element;
        parser->in_head = true;
    } else if (strcasecmp(token->tag_name, "body") == 0) {
        html_document_t *doc = (html_document_t*)parser->document;
        doc->body_element = element;
        parser->in_body = true;
    } else if (strcasecmp(token->tag_name, "script") == 0) {
        parser->in_script = true;
    } else if (strcasecmp(token->tag_name, "style") == 0) {
        parser->in_style = true;
    }
    
    // Push current node if not self-closing and not void element
    if (!token->self_closing && !html_is_void_element(token->tag_name)) {
        parser->current_node = &element->base;
    }
}

static void html_handle_end_tag(html_parser_t *parser, html_token_t *token) {
    // Find matching start tag
    dom_node_t *current = parser->current_node;
    while (current && current != &parser->document->base) {
        if (dom_node_is_element(current)) {
            dom_element_t *element = (dom_element_t*)current;
            if (strcasecmp(element->base.tag_name, token->tag_name) == 0) {
                // Found matching element
                parser->current_node = current->parent ? current->parent : parser->current_node;
                break;
            }
        }
        current = current->parent;
    }
    
    // Handle special elements
    if (strcasecmp(token->tag_name, "head") == 0) {
        parser->in_head = false;
    } else if (strcasecmp(token->tag_name, "body") == 0) {
        parser->in_body = false;
    } else if (strcasecmp(token->tag_name, "script") == 0) {
        parser->in_script = false;
    } else if (strcasecmp(token->tag_name, "style") == 0) {
        parser->in_style = false;
    }
}

static void html_handle_text(html_parser_t *parser, html_token_t *token) {
    if (!token->data || strlen(token->data) == 0) {
        return;
    }
    
    dom_text_t *text = dom_text_create(token->data);
    if (!text) return;
    
    dom_node_append_child(parser->current_node, &text->base);
}

html_document_t *html_parser_parse(html_parser_t *parser, const char *html) {
    if (!parser || !html) return NULL;
    
    parser->input = html;
    parser->length = strlen(html);
    parser->position = 0;
    parser->line = 1;
    parser->column = 1;
    
    // Reset state
    parser->in_head = false;
    parser->in_body = false;
    parser->in_script = false;
    parser->in_style = false;
    parser->has_errors = false;
    
    dom_node_t *root = html_build_dom_tree(parser);
    if (!root) return NULL;
    
    return (html_document_t*)root;
}

html_document_t *html_parser_parse_file(html_parser_t *parser, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        snprintf(parser->error_message, sizeof(parser->error_message), "Cannot open file: %s", filename);
        parser->has_errors = true;
        return NULL;
    }
    
    // Read file
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *html = malloc(size + 1);
    if (!html) {
        fclose(file);
        return NULL;
    }
    
    fread(html, 1, size, file);
    html[size] = '\0';
    fclose(file);
    
    html_document_t *doc = html_parser_parse(parser, html);
    free(html);
    
    return doc;
}

bool html_parser_has_errors(html_parser_t *parser) {
    return parser->has_errors;
}

const char *html_parser_get_error_message(html_parser_t *parser) {
    return parser->error_message;
}

// Utility functions
bool html_is_void_element(const char *tag_name) {
    if (!tag_name) return false;
    
    static const char *void_elements[] = {
        "area", "base", "br", "col", "embed", "hr", "img", "input",
        "keygen", "link", "meta", "param", "source", "track", "wbr",
        NULL
    };
    
    for (int i = 0; void_elements[i]; i++) {
        if (strcasecmp(tag_name, void_elements[i]) == 0) {
            return true;
        }
    }
    
    return false;
}

char *html_normalize_tag_name(const char *tag_name) {
    if (!tag_name) return NULL;
    
    size_t len = strlen(tag_name);
    char *normalized = malloc(len + 1);
    
    for (size_t i = 0; i < len; i++) {
        normalized[i] = tolower(tag_name[i]);
    }
    normalized[len] = '\0';
    
    return normalized;
}
