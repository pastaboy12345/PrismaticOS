#ifndef HTML_HTML_H
#define HTML_HTML_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <dom/dom.h>

// HTML token types
typedef enum {
    HTML_TOKEN_DOCTYPE,
    HTML_TOKEN_START_TAG,
    HTML_TOKEN_END_TAG,
    HTML_TOKEN_COMMENT,
    HTML_TOKEN_TEXT,
    HTML_TOKEN_EOF
} html_token_type_t;

// HTML token
typedef struct html_token {
    html_token_type_t type;
    
    // For tags
    char *tag_name;
    bool self_closing;
    
    // For attributes
    char **attr_names;
    char **attr_values;
    size_t attr_count;
    
    // For text/comment
    char *data;
    
    // Position information
    size_t line;
    size_t column;
    size_t offset;
} html_token_t;

// HTML parser
typedef struct html_parser {
    const char *input;
    size_t position;
    size_t length;
    size_t line;
    size_t column;
    
    // Token buffer
    html_token_t current_token;
    bool token_ready;
    
    // DOM document being built
    dom_document_t *document;
    dom_node_t *current_node;
    
    // Parser state
    bool in_head;
    bool in_body;
    bool in_script;
    bool in_style;
    
    // Error handling
    bool has_errors;
    char error_message[256];
} html_parser_t;

// HTML document
typedef struct html_document {
    dom_document_t base;  // Base DOM document
    
    // HTML-specific properties
    dom_element_t *html_element;
    dom_element_t *head_element;
    dom_element_t *body_element;
    
    // Forms
    dom_element_t **forms;
    size_t form_count;
    
    // Images
    dom_element_t **images;
    size_t image_count;
    
    // Links
    dom_element_t **links;
    size_t link_count;
    
    // Scripts
    dom_element_t **scripts;
    size_t script_count;
    
    // Stylesheets
    dom_element_t **stylesheets;
    size_t stylesheet_count;
    
    // Ready state
    bool dom_content_loaded;
    bool load_complete;
} html_document_t;

// HTML element
typedef struct html_element {
    dom_element_t base;  // Base DOM element
    
    // HTML-specific properties
    char *tag_name_lower;  // Lowercase tag name for case-insensitive comparison
    bool is_html_unknown;
    
    // Form-specific
    bool is_form_control;
    char *form_action;
    char *form_method;
    char *form_enctype;
    bool form_no_validate;
    
    // Input-specific
    char *input_type;
    char *input_value;
    char *input_placeholder;
    bool input_required;
    bool input_disabled;
    bool input_readonly;
    bool input_checked;
    bool input_multiple;
    
    // Link-specific
    char *link_href;
    char *link_rel;
    char *link_type;
    char *link_media;
    bool link_disabled;
    
    // Image-specific
    char *img_src;
    char *img_alt;
    char *img_srcset;
    char *img_sizes;
    bool img_is_map;
    
    // Script-specific
    char *script_src;
    char *script_type;
    char *script_charset;
    bool script_async;
    bool script_defer;
    bool script_cross_origin;
    
    // Style-specific
    char *style_media;
    bool style_scoped;
} html_element_t;

// HTML parser functions
html_parser_t *html_parser_create(void);
void html_parser_destroy(html_parser_t *parser);
html_document_t *html_parser_parse(html_parser_t *parser, const char *html);
html_document_t *html_parser_parse_file(html_parser_t *parser, const char *filename);
html_token_t *html_parser_next_token(html_parser_t *parser);
bool html_parser_has_errors(html_parser_t *parser);
const char *html_parser_get_error_message(html_parser_t *parser);

// HTML document functions
html_document_t *html_document_create(const char *url);
void html_document_destroy(html_document_t *doc);
dom_element_t *html_document_create_element(html_document_t *doc, const char *tag_name);
dom_element_t *html_document_create_element_ns(html_document_t *doc, const char *namespace, const char *tag_name);
dom_text_t *html_document_create_text_node(html_document_t *doc, const char *data);
dom_comment_t *html_document_create_comment(html_document_t *doc, const char *data);

// HTML element functions
html_element_t *html_element_create(const char *tag_name);
void html_element_set_attribute(html_element_t *element, const char *name, const char *value);
const char *html_element_get_attribute(html_element_t *element, const char *name);
bool html_element_has_attribute(html_element_t *element, const char *name);
void html_element_remove_attribute(html_element_t *element, const char *name);

// HTML collection
typedef struct html_collection {
    dom_node_t **nodes;
    size_t length;
    bool is_live;
    dom_node_t *root;
    char *selector;
} html_collection_t;

html_collection_t *html_collection_create(void);
void html_collection_destroy(html_collection_t *collection);
void html_collection_add_node(html_collection_t *collection, dom_node_t *node);
void html_collection_remove_node(html_collection_t *collection, dom_node_t *node);
dom_node_t *html_collection_item(html_collection_t *collection, size_t index);
size_t html_collection_get_length(html_collection_t *collection);

// HTML-specific DOM methods
html_collection_t *html_document_get_elements_by_tag_name(html_document_t *doc, const char *tag_name);
html_collection_t *html_document_get_elements_by_class_name(html_document_t *doc, const char *class_name);
html_collection_t *html_document_get_elements_by_name(html_document_t *doc, const char *name);
dom_element_t *html_document_get_element_by_id(html_document_t *doc, const char *id);
html_collection_t *html_document_get_images(html_document_t *doc);
html_collection_t *html_document_get_forms(html_document_t *doc);
html_collection_t *html_document_get_anchors(html_document_t *doc);
html_collection_t *html_document_get_links(html_document_t *doc);

// Form handling
typedef struct html_form_element html_form_element_t;
typedef struct html_input_element html_input_element_t;
typedef struct html_select_element html_select_element_t;
typedef struct html_textarea_element html_textarea_element_t;
typedef struct html_button_element html_button_element_t;

html_form_element_t *html_form_element_create(void);
void html_form_element_submit(html_form_element_t *form);
void html_form_element_reset(html_form_element_t *form);
bool html_form_element_check_validity(html_form_element_t *form);

html_input_element_t *html_input_element_create(const char *type);
const char *html_input_element_get_value(html_input_element_t *input);
void html_input_element_set_value(html_input_element_t *input, const char *value);
bool html_input_element_get_checked(html_input_element_t *input);
void html_input_element_set_checked(html_input_element_t *input, bool checked);
bool html_input_element_is_disabled(html_input_element_t *input);
void html_input_element_set_disabled(html_input_element_t *input, bool disabled);

// Canvas support
typedef struct html_canvas_element html_canvas_element_t;
typedef struct canvas_context_2d canvas_context_2d_t;

html_canvas_element_t *html_canvas_element_create(uint32_t width, uint32_t height);
canvas_context_2d_t *html_canvas_element_get_context_2d(html_canvas_element_t *canvas);
void html_canvas_element_set_size(html_canvas_element_t *canvas, uint32_t width, uint32_t height);

// Media elements
typedef struct html_image_element html_image_element_t;
typedef struct html_audio_element html_audio_element_t;
typedef struct html_video_element html_video_element_t;

html_image_element_t *html_image_element_create(void);
void html_image_element_set_src(html_image_element_t *img, const char *src);
const char *html_image_element_get_src(html_image_element_t *img);
void html_image_element_set_alt(html_image_element_t *img, const char *alt);
const char *html_image_element_get_alt(html_image_element_t *img);

// Script handling
typedef struct html_script_element html_script_element_t;

html_script_element_t *html_script_element_create(void);
void html_script_element_set_src(html_script_element_t *script, const char *src);
const char *html_script_element_get_src(html_script_element_t *script);
void html_script_element_set_type(html_script_element_t *script, const char *type);
const char *html_script_element_get_type(html_script_element_t *script);
bool html_script_element_is_async(html_script_element_t *script);
bool html_script_element_is_defer(html_script_element_t *script);

// Link handling
typedef struct html_link_element html_link_element_t;

html_link_element_t *html_link_element_create(void);
void html_link_element_set_href(html_link_element_t *link, const char *href);
const char *html_link_element_get_href(html_link_element_t *link);
void html_link_element_set_rel(html_link_element_t *link, const char *rel);
const char *html_link_element_get_rel(html_link_element_t *link);

// Style handling
typedef struct html_style_element html_style_element_t;

html_style_element_t *html_style_element_create(void);
const char *html_style_element_get_media(html_style_element_t *style);
bool html_style_element_is_scoped(html_style_element_t *style);

// Utility functions
bool html_is_void_element(const char *tag_name);
bool html_is_form_element(const char *tag_name);
bool html_is_table_element(const char *tag_name);
bool html_is_list_element(const char *tag_name);
char *html_normalize_tag_name(const char *tag_name);
bool html_is_self_closing_tag(const char *tag_name);

// HTML5 semantic elements
bool html_is_sectioning_content(const char *tag_name);
bool html_is_heading_content(const char *tag_name);
bool html_is_phrasing_content(const char *tag_name);
bool html_is_embedded_content(const char *tag_name);
bool html_is_interactive_content(const char *tag_name);

// HTML5 validation
bool html_validate_element(html_element_t *element);
bool html_validate_attribute(html_element_t *element, const char *name, const char *value);
char **html_get_validation_errors(html_element_t *element, size_t *error_count);

#endif // HTML_HTML_H
