#include <css/css.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

// CSS parser implementation
css_parser_t *css_parser_create(void) {
    css_parser_t *parser = malloc(sizeof(css_parser_t));
    if (!parser) return NULL;
    
    memset(parser, 0, sizeof(css_parser_t));
    
    return parser;
}

void css_parser_destroy(css_parser_t *parser) {
    if (!parser) return;
    
    // Clean up current token
    free(parser->current_token.value);
    
    free(parser);
}

// Token parsing functions
static char *css_parse_ident(css_parser_t *parser) {
    const char *input = parser->input + parser->position;
    size_t start = parser->position;
    
    // Parse identifier (starts with letter, hyphen, or underscore)
    if (parser->position < parser->length) {
        char c = parser->input[parser->position];
        if (!isalpha(c) && c != '-' && c != '_' && c != '\\') {
            return NULL;
        }
        parser->position++;
    }
    
    // Continue with letters, digits, hyphens, underscores
    while (parser->position < parser->length) {
        char c = parser->input[parser->position];
        if (!isalnum(c) && c != '-' && c != '_' && c != '\\') {
            break;
        }
        parser->position++;
    }
    
    if (start == parser->position) {
        return NULL;
    }
    
    size_t length = parser->position - start;
    char *ident = malloc(length + 1);
    memcpy(ident, input, length);
    ident[length] = '\0';
    
    return ident;
}

static char *css_parse_string(css_parser_t *parser, char quote) {
    parser->position++;  // Skip opening quote
    
    const char *start = parser->input + parser->position;
    
    while (parser->position < parser->length && parser->input[parser->position] != quote) {
        if (parser->input[parser->position] == '\\') {
            parser->position++;  // Skip escape character
            if (parser->position < parser->length) {
                parser->position++;  // Skip escaped character
            }
        } else {
            parser->position++;
        }
    }
    
    if (parser->position >= parser->length) {
        return NULL;  // Unterminated string
    }
    
    size_t length = parser->position - (start - parser->input);
    char *string = malloc(length + 1);
    memcpy(string, start, length);
    string[length] = '\0';
    
    parser->position++;  // Skip closing quote
    
    return string;
}

static char *css_parse_url(css_parser_t *parser) {
    parser->position += 3;  // Skip "url("
    
    // Skip whitespace
    while (parser->position < parser->length && isspace(parser->input[parser->position])) {
        parser->position++;
    }
    
    char quote = '\0';
    if (parser->position < parser->length) {
        char c = parser->input[parser->position];
        if (c == '"' || c == '\'') {
            quote = c;
        }
    }
    
    char *url;
    if (quote) {
        url = css_parse_string(parser, quote);
    } else {
        // Parse unquoted URL
        const char *start = parser->input + parser->position;
        
        while (parser->position < parser->length && parser->input[parser->position] != ')') {
            if (parser->input[parser->position] == '\\') {
                parser->position++;  // Skip escape character
                if (parser->position < parser->length) {
                    parser->position++;  // Skip escaped character
                }
            } else {
                parser->position++;
            }
        }
        
        if (parser->position >= parser->length) {
            return NULL;  // Unterminated URL
        }
        
        size_t length = parser->position - (start - parser->input);
        url = malloc(length + 1);
        memcpy(url, start, length);
        url[length] = '\0';
    }
    
    // Skip whitespace
    while (parser->position < parser->length && isspace(parser->input[parser->position])) {
        parser->position++;
    }
    
    // Expect ')'
    if (parser->position < parser->length && parser->input[parser->position] == ')') {
        parser->position++;
    } else {
        free(url);
        return NULL;  // Malformed URL
    }
    
    return url;
}

static css_token_t *css_next_token(css_parser_t *parser) {
    if (!parser || parser->position >= parser->length) {
        return NULL;
    }
    
    // Skip whitespace and comments
    while (parser->position < parser->length) {
        char c = parser->input[parser->position];
        
        if (isspace(c)) {
            parser->position++;
            continue;
        }
        
        // Skip comments
        if (parser->position + 1 < parser->length && c == '/' && parser->input[parser->position + 1] == '*') {
            parser->position += 2;
            while (parser->position + 1 < parser->length &&
                   !(parser->input[parser->position] == '*' && parser->input[parser->position + 1] == '/')) {
                parser->position++;
            }
            parser->position += 2;  // Skip "*/"
            continue;
        }
        
        break;
    }
    
    if (parser->position >= parser->length) {
        return NULL;
    }
    
    css_token_t *token = malloc(sizeof(css_token_t));
    memset(token, 0, sizeof(css_token_t));
    token->line = parser->line;
    token->column = parser->column;
    token->offset = parser->position;
    
    char c = parser->input[parser->position];
    
    // Single-character tokens
    switch (c) {
        case ':':
            token->type = CSS_TOKEN_COLON;
            parser->position++;
            return token;
        case ';':
            token->type = CSS_TOKEN_SEMICOLON;
            parser->position++;
            return token;
        case ',':
            token->type = CSS_TOKEN_COMMA;
            parser->position++;
            return token;
        case '(':
            token->type = CSS_TOKEN_LEFT_PAREN;
            parser->position++;
            return token;
        case ')':
            token->type = CSS_TOKEN_RIGHT_PAREN;
            parser->position++;
            return token;
        case '[':
            token->type = CSS_TOKEN_LEFT_BRACKET;
            parser->position++;
            return token;
        case ']':
            token->type = CSS_TOKEN_RIGHT_BRACKET;
            parser->position++;
            return token;
        case '{':
            token->type = CSS_TOKEN_LEFT_BRACE;
            parser->position++;
            return token;
        case '}':
            token->type = CSS_TOKEN_RIGHT_BRACE;
            parser->position++;
            return token;
        case '~':
            if (parser->position + 1 < parser->length && parser->input[parser->position + 1] == '=') {
                token->type = CSS_TOKEN_INCLUDE_MATCH;
                parser->position += 2;
                return token;
            }
            break;
        case '|':
            if (parser->position + 1 < parser->length) {
                if (parser->input[parser->position + 1] == '=') {
                    token->type = CSS_TOKEN_DASH_MATCH;
                    parser->position += 2;
                    return token;
                } else if (parser->input[parser->position + 1] == '|') {
                    token->type = CSS_TOKEN_COLUMN;
                    parser->position += 2;
                    return token;
                }
            }
            break;
        case '^':
            if (parser->position + 1 < parser->length && parser->input[parser->position + 1] == '=') {
                token->type = CSS_TOKEN_PREFIX_MATCH;
                parser->position += 2;
                return token;
            }
            break;
        case '$':
            if (parser->position + 1 < parser->length && parser->input[parser->position + 1] == '=') {
                token->type = CSS_TOKEN_SUFFIX_MATCH;
                parser->position += 2;
                return token;
            }
            break;
        case '*':
            if (parser->position + 1 < parser->length && parser->input[parser->position + 1] == '=') {
                token->type = CSS_TOKEN_SUBSTRING_MATCH;
                parser->position += 2;
                return token;
            }
            token->type = CSS_TOKEN_DELIM;
            token->value = malloc(2);
            token->value[0] = '*';
            token->value[1] = '\0';
            token->length = 1;
            parser->position++;
            return token;
    }
    
    // Multi-character tokens
    if (c == '@') {
        // At-keyword
        parser->position++;
        char *ident = css_parse_ident(parser);
        if (ident) {
            token->type = CSS_TOKEN_AT_KEYWORD;
            token->value = ident;
            token->length = strlen(ident);
            return token;
        }
    } else if (c == '#') {
        // Hash
        parser->position++;
        if (parser->position < parser->length && isxdigit(parser->input[parser->position])) {
            // Color or ID
            const char *start = parser->input + parser->position;
            while (parser->position < parser->length && isxdigit(parser->input[parser->position])) {
                parser->position++;
            }
            
            size_t length = parser->position - (start - parser->input);
            char *hash = malloc(length + 1);
            memcpy(hash, start, length);
            hash[length] = '\0';
            
            token->type = CSS_TOKEN_HASH;
            token->value = hash;
            token->length = length;
            return token;
        }
    } else if (c == '.') {
        // Number starting with dot or class selector
        if (parser->position + 1 < parser->length && isdigit(parser->input[parser->position + 1])) {
            // Number
            return css_parse_number(parser, token);
        } else {
            token->type = CSS_TOKEN_DELIM;
            token->value = malloc(2);
            token->value[0] = '.';
            token->value[1] = '\0';
            token->length = 1;
            parser->position++;
            return token;
        }
    } else if (c == '-' || isdigit(c)) {
        // Number or identifier starting with hyphen
        return css_parse_number_or_ident(parser, token);
    } else if (c == '"' || c == '\'') {
        // String
        char *string = css_parse_string(parser, c);
        if (string) {
            token->type = CSS_TOKEN_STRING;
            token->value = string;
            token->length = strlen(string);
            return token;
        }
    } else if (c == 'u' || c == 'U') {
        // URL or identifier
        if (parser->position + 3 < parser->length &&
            strncasecmp(parser->input + parser->position, "url(", 4) == 0) {
            char *url = css_parse_url(parser);
            if (url) {
                token->type = CSS_TOKEN_URL;
                token->value = url;
                token->length = strlen(url);
                return token;
            }
        } else {
            char *ident = css_parse_ident(parser);
            if (ident) {
                token->type = CSS_TOKEN_IDENT;
                token->value = ident;
                token->length = strlen(ident);
                return token;
            }
        }
    } else if (isalpha(c) || c == '_') {
        // Identifier
        char *ident = css_parse_ident(parser);
        if (ident) {
            token->type = CSS_TOKEN_IDENT;
            token->value = ident;
            token->length = strlen(ident);
            return token;
        }
    }
    
    // Fallback to delimiter
    token->type = CSS_TOKEN_DELIM;
    token->value = malloc(2);
    token->value[0] = c;
    token->value[1] = '\0';
    token->length = 1;
    parser->position++;
    
    return token;
}

static css_token_t *css_parse_number(css_parser_t *parser, css_token_t *token) {
    const char *start = parser->input + parser->position;
    
    // Parse sign
    if (parser->input[parser->position] == '+' || parser->input[parser->position] == '-') {
        parser->position++;
    }
    
    // Parse digits
    while (parser->position < parser->length && isdigit(parser->input[parser->position])) {
        parser->position++;
    }
    
    // Parse decimal point
    bool has_decimal = false;
    if (parser->position < parser->length && parser->input[parser->position] == '.') {
        has_decimal = true;
        parser->position++;
        while (parser->position < parser->length && isdigit(parser->input[parser->position])) {
            parser->position++;
        }
    }
    
    // Parse exponent
    bool has_exponent = false;
    if (parser->position < parser->length && 
        (parser->input[parser->position] == 'e' || parser->input[parser->position] == 'E')) {
        has_exponent = true;
        parser->position++;
        if (parser->position < parser->length && 
            (parser->input[parser->position] == '+' || parser->input[parser->position] == '-')) {
            parser->position++;
        }
        while (parser->position < parser->length && isdigit(parser->input[parser->position])) {
            parser->position++;
        }
    }
    
    size_t length = parser->position - (start - parser->input);
    char *number_str = malloc(length + 1);
    memcpy(number_str, start, length);
    number_str[length] = '\0';
    
    // Determine token type
    if (has_decimal || has_exponent) {
        token->type = CSS_TOKEN_NUMBER;
        token->data.number = atof(number_str);
    } else {
        token->type = CSS_TOKEN_INTEGER;
        token->data.number = atoi(number_str);
    }
    
    token->value = number_str;
    token->length = length;
    
    return token;
}

// CSS parsing functions
css_stylesheet_t *css_parser_parse(css_parser_t *parser, const char *css) {
    if (!parser || !css) return NULL;
    
    parser->input = css;
    parser->length = strlen(css);
    parser->position = 0;
    parser->line = 1;
    parser->column = 1;
    
    css_stylesheet_t *stylesheet = css_stylesheet_create(NULL);
    if (!stylesheet) return NULL;
    
    css_token_t *token;
    while ((token = css_next_token(parser)) != NULL) {
        // Parse rules based on token sequence
        // This is a simplified parser - a full CSS parser would be much more complex
        css_parse_rule(parser, stylesheet, token);
        
        free(token->value);
        free(token);
    }
    
    return stylesheet;
}

css_stylesheet_t *css_parser_parse_file(css_parser_t *parser, const char *filename) {
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
    
    char *css = malloc(size + 1);
    if (!css) {
        fclose(file);
        return NULL;
    }
    
    fread(css, 1, size, file);
    css[size] = '\0';
    fclose(file);
    
    css_stylesheet_t *stylesheet = css_parser_parse(parser, css);
    free(css);
    
    return stylesheet;
}

bool css_parser_has_errors(css_parser_t *parser) {
    return parser->has_errors;
}

const char *css_parser_get_error_message(css_parser_t *parser) {
    return parser->error_message;
}

// Simplified rule parsing
static void css_parse_rule(css_parser_t *parser, css_stylesheet_t *stylesheet, css_token_t *first_token) {
    // This is a very simplified CSS parser
    // A full implementation would handle complex selectors, at-rules, etc.
    
    if (first_token->type == CSS_TOKEN_AT_KEYWORD) {
        // At-rule (simplified)
        css_parse_at_rule(parser, stylesheet, first_token);
    } else if (first_token->type == CSS_TOKEN_IDENT || first_token->type == CSS_TOKEN_HASH) {
        // Style rule (simplified)
        css_parse_style_rule(parser, stylesheet, first_token);
    }
}

static void css_parse_at_rule(css_parser_t *parser, css_stylesheet_t *stylesheet, css_token_t *at_keyword) {
    // Skip to semicolon or opening brace
    css_token_t *token;
    while ((token = css_next_token(parser)) != NULL) {
        if (token->type == CSS_TOKEN_SEMICOLON || token->type == CSS_TOKEN_LEFT_BRACE) {
            break;
        }
        free(token->value);
        free(token);
    }
    
    // Handle opening brace for nested rules
    if (token && token->type == CSS_TOKEN_LEFT_BRACE) {
        // Skip nested content
        int brace_depth = 1;
        while ((token = css_next_token(parser)) != NULL) {
            if (token->type == CSS_TOKEN_LEFT_BRACE) {
                brace_depth++;
            } else if (token->type == CSS_TOKEN_RIGHT_BRACE) {
                brace_depth--;
                if (brace_depth == 0) {
                    break;
                }
            }
            free(token->value);
            free(token);
        }
    }
    
    if (token) {
        free(token->value);
        free(token);
    }
}

static void css_parse_style_rule(css_parser_t *parser, css_stylesheet_t *stylesheet, css_token_t *first_token) {
    // Create a simple rule with the first token as selector
    css_rule_t *rule = css_rule_create(NULL);
    if (!rule) return;
    
    // Add selector (simplified - just use the first token)
    css_selector_t *selector = css_selector_create(CSS_SELECTOR_TYPE_ELEMENT, first_token->value);
    if (selector) {
        selector->next = rule->selectors;
        rule->selectors = selector;
    }
    
    // Skip to opening brace
    css_token_t *token;
    while ((token = css_next_token(parser)) != NULL) {
        if (token->type == CSS_TOKEN_LEFT_BRACE) {
            break;
        }
        free(token->value);
        free(token);
    }
    
    if (!token) {
        css_rule_destroy(rule);
        return;
    }
    free(token->value);
    free(token);
    
    // Parse declarations until closing brace
    while ((token = css_next_token(parser)) != NULL) {
        if (token->type == CSS_TOKEN_RIGHT_BRACE) {
            break;
        }
        
        if (token->type == CSS_TOKEN_IDENT) {
            css_parse_declaration(parser, rule, token);
        }
        
        free(token->value);
        free(token);
    }
    
    css_stylesheet_add_rule(stylesheet, rule);
}

static void css_parse_declaration(css_parser_t *parser, css_rule_t *rule, css_token_t *property_token) {
    // Expect colon
    css_token_t *token = css_next_token(parser);
    if (!token || token->type != CSS_TOKEN_COLON) {
        if (token) {
            free(token->value);
            free(token);
        }
        return;
    }
    free(token->value);
    free(token);
    
    // Parse value
    char *value = NULL;
    while ((token = css_next_token(parser)) != NULL) {
        if (token->type == CSS_TOKEN_SEMICOLON || token->type == CSS_TOKEN_RIGHT_BRACE) {
            break;
        }
        
        // Concatenate value tokens (simplified)
        if (value) {
            size_t old_len = strlen(value);
            size_t new_len = strlen(token->value);
            value = realloc(value, old_len + new_len + 2);
            strcat(value, " ");
            strcat(value, token->value);
        } else {
            value = strdup(token->value);
        }
        
        free(token->value);
        free(token);
    }
    
    if (value && property_token->value) {
        css_declaration_t *declaration = css_declaration_create();
        if (declaration) {
            declaration->property = strdup(property_token->value);
            declaration->value = css_value_parse_string(value);
            css_rule_add_declaration(rule, declaration);
        }
        free(value);
    }
    
    if (token && token->type != CSS_TOKEN_SEMICOLON && token->type != CSS_TOKEN_RIGHT_BRACE) {
        free(token->value);
        free(token);
    }
}

// CSS stylesheet functions
css_stylesheet_t *css_stylesheet_create(const char *url) {
    css_stylesheet_t *stylesheet = malloc(sizeof(css_stylesheet_t));
    if (!stylesheet) return NULL;
    
    memset(stylesheet, 0, sizeof(css_stylesheet_t));
    
    if (url) {
        stylesheet->url = strdup(url);
    }
    
    return stylesheet;
}

void css_stylesheet_destroy(css_stylesheet_t *stylesheet) {
    if (!stylesheet) return;
    
    free(stylesheet->url);
    free(stylesheet->title);
    
    // Destroy rules
    css_rule_t *rule = stylesheet->rules;
    while (rule) {
        css_rule_t *next = rule->next;
        css_rule_destroy(rule);
        rule = next;
    }
    
    free(stylesheet);
}

void css_stylesheet_add_rule(css_stylesheet_t *stylesheet, css_rule_t *rule) {
    if (!stylesheet || !rule) return;
    
    rule->next = stylesheet->rules;
    stylesheet->rules = rule;
}

// CSS rule functions
css_rule_t *css_rule_create(css_selector_t *selectors) {
    css_rule_t *rule = malloc(sizeof(css_rule_t));
    if (!rule) return NULL;
    
    memset(rule, 0, sizeof(css_rule_t));
    rule->selectors = selectors;
    
    // Calculate specificity
    if (selectors) {
        rule->specificity = css_selector_calculate_specificity(selectors);
    }
    
    return rule;
}

void css_rule_destroy(css_rule_t *rule) {
    if (!rule) return;
    
    // Destroy selectors
    css_selector_t *selector = rule->selectors;
    while (selector) {
        css_selector_t *next = selector->next;
        css_selector_destroy(selector);
        selector = next;
    }
    
    // Destroy declarations
    css_declaration_t *declaration = rule->declarations;
    while (declaration) {
        css_declaration_t *next = declaration->next;
        css_declaration_destroy(declaration);
        declaration = next;
    }
    
    free(rule);
}

void css_rule_add_declaration(css_rule_t *rule, css_declaration_t *declaration) {
    if (!rule || !declaration) return;
    
    declaration->next = rule->declarations;
    rule->declarations = declaration;
}

// CSS selector functions
css_selector_t *css_selector_create(css_selector_type_t type, const char *value) {
    css_selector_t *selector = malloc(sizeof(css_selector_t));
    if (!selector) return NULL;
    
    memset(selector, 0, sizeof(css_selector_t));
    selector->type = type;
    
    if (value) {
        selector->value = strdup(value);
    }
    
    return selector;
}

void css_selector_destroy(css_selector_t *selector) {
    if (!selector) return;
    
    free(selector->value);
    free(selector->attr_name);
    free(selector->attr_value);
    
    // Recursively destroy next selector
    if (selector->next) {
        css_selector_destroy(selector->next);
    }
    
    free(selector);
}

uint32_t css_selector_calculate_specificity(css_selector_t *selector) {
    if (!selector) return 0;
    
    uint32_t specificity = 0;
    
    // Count IDs (1000 each)
    // Count classes, attributes, pseudo-classes (100 each)
    // Count elements and pseudo-elements (10 each)
    
    while (selector) {
        switch (selector->type) {
            case CSS_SELECTOR_TYPE_ID:
                specificity += 1000;
                break;
            case CSS_SELECTOR_TYPE_CLASS:
            case CSS_SELECTOR_TYPE_ATTRIBUTE:
            case CSS_SELECTOR_TYPE_PSEUDO_CLASS:
                specificity += 100;
                break;
            case CSS_SELECTOR_TYPE_ELEMENT:
            case CSS_SELECTOR_TYPE_PSEUDO_ELEMENT:
                specificity += 10;
                break;
            default:
                break;
        }
        
        selector = selector->next;
    }
    
    return specificity;
}

// CSS value functions
css_value_t *css_value_create(css_value_type_t type) {
    css_value_t *value = malloc(sizeof(css_value_t));
    if (!value) return NULL;
    
    memset(value, 0, sizeof(css_value_t));
    value->type = type;
    
    return value;
}

void css_value_destroy(css_value_t *value) {
    if (!value) return;
    
    switch (value->type) {
        case CSS_VALUE_STRING:
        free(value->data.string);
            break;
        case CSS_VALUE_DIMENSION:
            free(value->data.dimension.unit);
            break;
        case CSS_VALUE_FUNCTION:
            free(value->data.function.name);
            for (size_t i = 0; i < value->data.function.arg_count; i++) {
                css_value_destroy(value->data.function.arguments[i]);
            }
            free(value->data.function.arguments);
            break;
        case CSS_VALUE_KEYWORD:
            free(value->data.keyword);
            break;
        default:
            break;
    }
    
    free(value);
}

css_value_t *css_value_parse_string(const char *str) {
    if (!str) return NULL;
    
    css_value_t *value = css_value_create(CSS_VALUE_STRING);
    if (!value) return NULL;
    
    value->data.string = strdup(str);
    return value;
}

css_value_t *css_value_parse_length(const char *str) {
    if (!str) return NULL;
    
    css_value_t *value = css_value_create(CSS_VALUE_LENGTH);
    if (!value) return NULL;
    
    // Parse number part
    char *endptr;
    value->data.dimension.value = strtod(str, &endptr);
    
    // Parse unit part
    while (*endptr && isspace(*endptr)) {
        endptr++;
    }
    
    if (*endptr) {
        value->data.dimension.unit = strdup(endptr);
    } else {
        value->data.dimension.unit = strdup("px");
    }
    
    return value;
}

css_value_t *css_value_parse_color(const char *str) {
    if (!str) return NULL;
    
    css_value_t *value = css_value_create(CSS_VALUE_COLOR);
    if (!value) return NULL;
    
    // Parse hex color
    if (str[0] == '#') {
        uint32_t color = strtoul(str + 1, NULL, 16);
        
        if (strlen(str) == 4) {
            // #RGB format
            value->data.color.r = ((color >> 8) & 0xF) * 17;
            value->data.color.g = ((color >> 4) & 0xF) * 17;
            value->data.color.b = (color & 0xF) * 17;
            value->data.color.a = 255;
        } else if (strlen(str) == 5) {
            // #RGBA format
            value->data.color.r = ((color >> 12) & 0xF) * 17;
            value->data.color.g = ((color >> 8) & 0xF) * 17;
            value->data.color.b = ((color >> 4) & 0xF) * 17;
            value->data.color.a = (color & 0xF) * 17;
        } else if (strlen(str) == 7) {
            // #RRGGBB format
            value->data.color.r = (color >> 16) & 0xFF;
            value->data.color.g = (color >> 8) & 0xFF;
            value->data.color.b = color & 0xFF;
            value->data.color.a = 255;
        } else if (strlen(str) == 9) {
            // #RRGGBBAA format
            value->data.color.r = (color >> 24) & 0xFF;
            value->data.color.g = (color >> 16) & 0xFF;
            value->data.color.b = (color >> 8) & 0xFF;
            value->data.color.a = color & 0xFF;
        }
    }
    
    return value;
}

css_value_t *css_value_parse_url(const char *str) {
    if (!str) return NULL;
    
    css_value_t *value = css_value_create(CSS_VALUE_URL);
    if (!value) return NULL;
    
    value->data.string = strdup(str);
    return value;
}
