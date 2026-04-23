#ifndef CSS_CSS_H
#define CSS_CSS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <dom/dom.h>

// CSS token types
typedef enum {
    CSS_TOKEN_IDENT,
    CSS_TOKEN_AT_KEYWORD,
    CSS_TOKEN_STRING,
    CSS_TOKEN_HASH,
    CSS_TOKEN_NUMBER,
    CSS_TOKEN_PERCENTAGE,
    CSS_TOKEN_DIMENSION,
    CSS_TOKEN_URL,
    CSS_TOKEN_BAD_STRING,
    CSS_TOKEN_BAD_URL,
    CSS_TOKEN_DELIM,
    CSS_TOKEN_COLON,
    CSS_TOKEN_SEMICOLON,
    CSS_TOKEN_COMMA,
    CSS_TOKEN_LEFT_BRACKET,
    CSS_TOKEN_RIGHT_BRACKET,
    CSS_TOKEN_LEFT_PAREN,
    CSS_TOKEN_RIGHT_PAREN,
    CSS_TOKEN_LEFT_BRACE,
    CSS_TOKEN_RIGHT_BRACE,
    CSS_TOKEN_INCLUDE_MATCH,
    CSS_TOKEN_DASH_MATCH,
    CSS_TOKEN_PREFIX_MATCH,
    CSS_TOKEN_SUFFIX_MATCH,
    CSS_TOKEN_SUBSTRING_MATCH,
    CSS_TOKEN_COLUMN,
    CSS_TOKEN_WHITESPACE,
    CSS_TOKEN_CDO,
    CSS_TOKEN_CDC,
    CSS_TOKEN_EOF
} css_token_type_t;

// CSS token
typedef struct css_token {
    css_token_type_t type;
    char *value;
    size_t length;
    
    // Position information
    size_t line;
    size_t column;
    size_t offset;
} css_token_t;

// CSS value types
typedef enum {
    CSS_VALUE_INHERIT,
    CSS_VALUE_INITIAL,
    CSS_VALUE_UNSET,
    CSS_VALUE_REVERT,
    CSS_VALUE_STRING,
    CSS_VALUE_URL,
    CSS_VALUE_INTEGER,
    CSS_VALUE_NUMBER,
    CSS_VALUE_LENGTH,
    CSS_VALUE_PERCENTAGE,
    CSS_VALUE_COLOR,
    CSS_VALUE_KEYWORD,
    CSS_VALUE_FUNCTION,
    CSS_VALUE_CALC,
    CSS_VALUE_VAR
} css_value_type_t;

// CSS value
typedef struct css_value {
    css_value_type_t type;
    union {
        char *string;
        double number;
        struct {
            double value;
            char *unit;
        } dimension;
        struct {
            uint8_t r, g, b, a;
        } color;
        struct {
            char *name;
            css_value_t **arguments;
            size_t arg_count;
        } function;
        char *keyword;
    } data;
    
    // Important flag
    bool important;
} css_value_t;

// CSS selector types
typedef enum {
    CSS_SELECTOR_TYPE_ELEMENT,
    CSS_SELECTOR_TYPE_ID,
    CSS_SELECTOR_TYPE_CLASS,
    CSS_SELECTOR_TYPE_ATTRIBUTE,
    CSS_SELECTOR_TYPE_PSEUDO_CLASS,
    CSS_SELECTOR_TYPE_PSEUDO_ELEMENT,
    CSS_SELECTOR_TYPE_UNIVERSAL
} css_selector_type_t;

// CSS selector
typedef struct css_selector {
    css_selector_type_t type;
    char *value;
    
    // Attribute selectors
    char *attr_name;
    char *attr_value;
    enum {
        CSS_ATTR_MATCH_EXISTS,
        CSS_ATTR_MATCH_EQUALS,
        CSS_ATTR_MATCH_CONTAINS_WORD,
        CSS_ATTR_MATCH_STARTS_WITH,
        CSS_ATTR_MATCH_ENDS_WITH,
        CSS_ATTR_MATCH_CONTAINS
    } attr_matcher;
    
    // Pseudo-classes/pseudo-elements
    bool is_pseudo_element;
    css_selector_t *argument_selector;
    
    struct css_selector *next;  // For compound selectors
    struct css_selector *combinator;  // For combinators
    enum {
        CSS_COMBINATOR_DESCENDANT,
        CSS_COMBINATOR_CHILD,
        CSS_COMBINATOR_ADJACENT_SIBLING,
        CSS_COMBINATOR_GENERAL_SIBLING
    } combinator_type;
} css_selector_t;

// CSS declaration
typedef struct css_declaration {
    char *property;
    css_value_t *value;
    bool important;
    struct css_declaration *next;
} css_declaration_t;

// CSS rule
typedef struct css_rule {
    css_selector_t *selectors;
    css_declaration_t *declarations;
    
    // Specificity
    uint32_t specificity;
    
    // Media query
    struct css_media_query *media_query;
    
    struct css_rule *next;
} css_rule_t;

// CSS at-rule
typedef struct css_at_rule {
    char *name;
    char *prelude;
    struct css_rule *rules;  // For nested rules
    css_declaration_t *declarations;
    
    struct css_at_rule *next;
} css_at_rule_t;

// CSS stylesheet
typedef struct css_stylesheet {
    char *url;
    char *title;
    css_rule_t *rules;
    css_at_rule_t *at_rules;
    
    // Import rules
    struct css_import_rule *imports;
    
    // Media rules
    struct css_media_rule *media_rules;
    
    // Keyframe rules
    struct css_keyframe_rule *keyframe_rules;
    
    // Font-face rules
    struct css_font_face_rule *font_face_rules;
} css_stylesheet_t;

// CSS parser
typedef struct css_parser {
    const char *input;
    size_t position;
    size_t length;
    size_t line;
    size_t column;
    
    // Token buffer
    css_token_t current_token;
    bool token_ready;
    
    // Parser state
    bool in_declaration;
    bool in_at_rule;
    bool in_media_rule;
    
    // Error handling
    bool has_errors;
    char error_message[256];
} css_parser_t;

// CSS computed style
typedef struct css_computed_style {
    // Display properties
    char *display;
    char *position;
    char *float;
    char *clear;
    
    // Box properties
    double width, height;
    double min_width, min_height;
    double max_width, max_height;
    
    // Margin
    double margin_top, margin_right, margin_bottom, margin_left;
    
    // Padding
    double padding_top, padding_right, padding_bottom, padding_left;
    
    // Border
    double border_top_width, border_right_width, border_bottom_width, border_left_width;
    char *border_top_style, *border_right_style, *border_bottom_style, *border_left_style;
    struct { uint8_t r, g, b, a; } border_top_color, border_right_color, border_bottom_color, border_left_color;
    
    // Background
    struct { uint8_t r, g, b, a; } background_color;
    char *background_image;
    char *background_repeat;
    char *background_position;
    char *background_size;
    char *background_attachment;
    char *background_clip;
    char *background_origin;
    
    // Font
    char *font_family;
    double font_size;
    char *font_style;
    char *font_weight;
    char *font_variant;
    double line_height;
    
    // Text
    struct { uint8_t r, g, b, a; } color;
    char *text_align;
    char *text_decoration;
    char *text_transform;
    char *text_indent;
    char *white_space;
    double word_spacing;
    double letter_spacing;
    
    // Visibility
    char *visibility;
    double opacity;
    
    // Z-index
    int z_index;
    
    // Overflow
    char *overflow_x, *overflow_y;
    
    // Flexbox (simplified)
    char *flex_direction;
    char *flex_wrap;
    char *justify_content;
    char *align_items;
    char *align_content;
    double flex_grow, flex_shrink;
    double flex_basis;
    
    // Grid (simplified)
    char *grid_template_columns;
    char *grid_template_rows;
    char *grid_auto_flow;
    
    // Transforms
    char *transform;
    char *transform_origin;
    
    // Transitions
    char *transition_property;
    double transition_duration;
    char *transition_timing_function;
    double transition_delay;
    
    // Animations
    char *animation_name;
    double animation_duration;
    char *animation_timing_function;
    double animation_delay;
    int animation_iteration_count;
    char *animation_direction;
    char *animation_fill_mode;
    char *animation_play_state;
    
    // Custom properties (CSS variables)
    char **custom_property_names;
    css_value_t **custom_property_values;
    size_t custom_property_count;
} css_computed_style_t;

// CSS media query
typedef struct css_media_query {
    char *media_type;
    struct {
        char *feature;
        css_value_t *value;
    } *features;
    size_t feature_count;
    bool not;
    struct css_media_query *next;
} css_media_query_t;

// CSS parser functions
css_parser_t *css_parser_create(void);
void css_parser_destroy(css_parser_t *parser);
css_stylesheet_t *css_parser_parse(css_parser_t *parser, const char *css);
css_stylesheet_t *css_parser_parse_file(css_parser_t *parser, const char *filename);
css_token_t *css_parser_next_token(css_parser_t *parser);
bool css_parser_has_errors(css_parser_t *parser);
const char *css_parser_get_error_message(css_parser_t *parser);

// CSS stylesheet functions
css_stylesheet_t *css_stylesheet_create(const char *url);
void css_stylesheet_destroy(css_stylesheet_t *stylesheet);
void css_stylesheet_add_rule(css_stylesheet_t *stylesheet, css_rule_t *rule);
void css_stylesheet_add_at_rule(css_stylesheet_t *stylesheet, css_at_rule_t *at_rule);

// CSS rule functions
css_rule_t *css_rule_create(css_selector_t *selectors);
void css_rule_destroy(css_rule_t *rule);
void css_rule_add_declaration(css_rule_t *rule, css_declaration_t *declaration);
css_declaration_t *css_rule_get_declaration(css_rule_t *rule, const char *property);

// CSS selector functions
css_selector_t *css_selector_create(css_selector_type_t type, const char *value);
void css_selector_destroy(css_selector_t *selector);
uint32_t css_selector_calculate_specificity(css_selector_t *selector);
bool css_selector_matches(css_selector_t *selector, dom_node_t *element);
bool css_selector_matches_tree(css_selector_t *selector, dom_node_t *element, dom_node_t *root);

// CSS value functions
css_value_t *css_value_create(css_value_type_t type);
void css_value_destroy(css_value_t *value);
css_value_t *css_value_parse_string(const char *str);
css_value_t *css_value_parse_length(const char *str);
css_value_t *css_value_parse_color(const char *str);
css_value_t *css_value_parse_url(const char *str);

// CSS computed style functions
css_computed_style_t *css_computed_style_create(void);
void css_computed_style_destroy(css_computed_style_t *style);
void css_computed_style_set_property(css_computed_style_t *style, const char *property, css_value_t *value);
css_value_t *css_computed_style_get_property(css_computed_style_t *style, const char *property);
void css_computed_style_inherit(css_computed_style_t *style, css_computed_style_t *parent);

// CSS style resolution
css_computed_style_t *css_resolve_style(dom_node_t *node, css_stylesheet_t *stylesheet);
css_computed_style_t *css_resolve_style_with_parent(dom_node_t *node, css_stylesheet_t *stylesheet, css_computed_style_t *parent_style);
css_rule_t **css_get_matching_rules(dom_node_t *node, css_stylesheet_t *stylesheet, size_t *count);
void css_sort_rules_by_specificity(css_rule_t **rules, size_t count);

// CSS cascade and inheritance
void css_apply_cascade(css_computed_style_t *style, css_rule_t **rules, size_t rule_count);
void css_apply_inheritance(css_computed_style_t *style, css_computed_style_t *parent_style);
void css_apply_initial_values(css_computed_style_t *style);

// CSS units and conversion
double css_length_to_pixels(css_value_t *length, css_computed_style_t *style, double font_size);
double css_percentage_to_pixels(css_value_t *percentage, double reference);
bool css_color_parse(const char *str, uint8_t *r, uint8_t *g, uint8_t *b, uint8_t *a);
char *css_color_to_string(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

// CSS media queries
css_media_query_t *css_media_query_parse(const char *str);
void css_media_query_destroy(css_media_query_t *query);
bool css_media_query_matches(css_media_query_t *query, const char *media_type, int width, int height);

// CSS animation support
typedef struct css_animation {
    char *name;
    double duration;
    char *timing_function;
    double delay;
    int iteration_count;
    char *direction;
    char *fill_mode;
    char *play_state;
    double current_time;
    bool is_running;
} css_animation_t;

css_animation_t *css_animation_create(const char *name);
void css_animation_destroy(css_animation_t *animation);
void css_animation_start(css_animation_t *animation);
void css_animation_pause(css_animation_t *animation);
void css_animation_stop(css_animation_t *animation);

// CSS transition support
typedef struct css_transition {
    char *property;
    double duration;
    char *timing_function;
    double delay;
    css_value_t *start_value;
    css_value_t *end_value;
    double progress;
    bool is_running;
} css_transition_t;

css_transition_t *css_transition_create(const char *property);
void css_transition_destroy(css_transition_t *transition);
void css_transition_start(css_transition_t *transition, css_value_t *start, css_value_t *end);

// CSS utility functions
bool css_property_is_inherited(const char *property);
css_value_t *css_property_get_initial_value(const char *property);
bool css_property_supports_animation(const char *property);
bool css_property_supports_transition(const char *property);

// CSS validation
bool css_validate_selector(css_selector_t *selector);
bool css_validate_declaration(css_declaration_t *declaration);
bool css_validate_value(css_value_t *value, const char *property);
char **css_get_validation_errors(css_stylesheet_t *stylesheet, size_t *error_count);

#endif // CSS_CSS_H
