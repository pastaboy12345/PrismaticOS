#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <html/html.h>
#include <css/css.h>
#include <dom/dom.h>
#include <layout/layout.h>

// HTML/CSS test
static void test_html_parsing(void) {
    printf("Testing HTML parsing...\n");
    
    html_parser_t *parser = html_parser_create();
    if (!parser) {
        printf("  Parser creation: FAIL\n");
        return;
    }
    
    const char *html = 
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "    <title>Test Page</title>\n"
        "</head>\n"
        "<body>\n"
        "    <h1>Hello World</h1>\n"
        "    <p>This is a test.</p>\n"
        "    <div class=\"container\">\n"
        "        <span id=\"test\">Content</span>\n"
        "    </div>\n"
        "</body>\n"
        "</html>";
    
    html_document_t *doc = html_parser_parse(parser, html);
    printf("  HTML parsing: %s\n", doc ? "PASS" : "FAIL");
    
    if (doc) {
        // Test document structure
        if (doc->html_element && doc->head_element && doc->body_element) {
            printf("  Document structure: PASS\n");
        } else {
            printf("  Document structure: FAIL\n");
        }
        
        // Test element by ID
        dom_node_t *test_element = dom_document_get_element_by_id(doc, "test");
        printf("  Element by ID: %s\n", test_element ? "PASS" : "FAIL");
        
        if (test_element && dom_node_is_element(test_element)) {
            const char *text_content = dom_node_get_text_content(test_element);
            printf("  Element content: \"%s\" %s\n", text_content ? text_content : "NULL",
                   (text_content && strcmp(text_content, "Content") == 0) ? "PASS" : "FAIL");
        }
        
        html_document_destroy(doc);
    }
    
    html_parser_destroy(parser);
    printf("HTML parsing test completed\n\n");
}

static void test_dom_manipulation(void) {
    printf("Testing DOM manipulation...\n");
    
    // Create a simple DOM tree
    dom_document_t *doc = html_document_create("test://example.com");
    dom_element_t *root = dom_element_create("div");
    dom_node_append_child(&doc->base, &root->base);
    
    // Add children
    dom_element_t *child1 = dom_element_create("p");
    dom_text_t *text1 = dom_text_create("First paragraph");
    dom_node_append_child(&child1->base, &text1->base);
    dom_node_append_child(&root->base, &child1->base);
    
    dom_element_t *child2 = dom_element_create("p");
    dom_text_t *text2 = dom_text_create("Second paragraph");
    dom_node_append_child(&child2->base, &text2->base);
    dom_node_append_child(&root->base, &child2->base);
    
    printf("  DOM tree creation: PASS\n");
    
    // Test traversal
    int child_count = 0;
    for (dom_node_t *child = root->base.first_child; child; child = child->next_sibling) {
        child_count++;
    }
    printf("  Child count: %d %s\n", child_count, (child_count == 2) ? "PASS" : "FAIL");
    
    // Test attributes
    dom_node_set_attribute(&root->base, "id", "container");
    dom_node_set_attribute(&root->base, "class", "wrapper");
    
    const char *id = dom_node_get_attribute(&root->base, "id");
    const char *class_attr = dom_node_get_attribute(&root->base, "class");
    printf("  Attribute setting: %s\n", 
           (id && class_attr && strcmp(id, "container") == 0 && strcmp(class_attr, "wrapper") == 0) ? "PASS" : "FAIL");
    
    // Test classes
    dom_node_add_class(&root->base, "main");
    dom_node_add_class(&root->base, "content");
    
    bool has_class = dom_node_has_class(&root->base, "main");
    bool has_wrapper = dom_node_has_class(&root->base, "wrapper");
    printf("  Class management: %s\n", (has_class && has_wrapper) ? "PASS" : "FAIL");
    
    // Test text content
    const char *content = dom_node_get_text_content(&root->base);
    printf("  Text content: %s\n", 
           (content && strstr(content, "First paragraph") && strstr(content, "Second paragraph")) ? "PASS" : "FAIL");
    
    html_document_destroy(doc);
    printf("DOM manipulation test completed\n\n");
}

static void test_css_parsing(void) {
    printf("Testing CSS parsing...\n");
    
    css_parser_t *parser = css_parser_create();
    if (!parser) {
        printf("  Parser creation: FAIL\n");
        return;
    }
    
    const char *css = 
        "body {\n"
        "    font-family: Arial, sans-serif;\n"
        "    margin: 0;\n"
        "    padding: 20px;\n"
        "    background-color: #f0f0f0;\n"
        "}\n"
        ".container {\n"
        "    width: 800px;\n"
        "    margin: 0 auto;\n"
        "    background: white;\n"
        "    border: 1px solid #ccc;\n"
        "}\n"
        "#header {\n"
        "    height: 60px;\n"
        "    background: linear-gradient(to bottom, #333, #666);\n"
        "    color: white;\n"
        "}\n"
        "p {\n"
        "    line-height: 1.6;\n"
        "    margin-bottom: 1em;\n"
        "}";
    
    css_stylesheet_t *stylesheet = css_parser_parse(parser, css);
    printf("  CSS parsing: %s\n", stylesheet ? "PASS" : "FAIL");
    
    if (stylesheet) {
        // Count rules
        int rule_count = 0;
        css_rule_t *rule = stylesheet->rules;
        while (rule) {
            rule_count++;
            rule = rule->next;
        }
        printf("  Rule count: %d %s\n", rule_count, (rule_count >= 4) ? "PASS" : "FAIL");
        
        css_stylesheet_destroy(stylesheet);
    }
    
    css_parser_destroy(parser);
    printf("CSS parsing test completed\n\n");
}

static void test_css_selectors(void) {
    printf("Testing CSS selectors...\n");
    
    // Test element selector
    css_selector_t *element_sel = css_selector_create(CSS_SELECTOR_TYPE_ELEMENT, "div");
    uint32_t specificity = css_selector_calculate_specificity(element_sel);
    printf("  Element selector specificity: %u %s\n", specificity, (specificity == 10) ? "PASS" : "FAIL");
    
    // Test class selector
    css_selector_t *class_sel = css_selector_create(CSS_SELECTOR_TYPE_CLASS, "container");
    specificity = css_selector_calculate_specificity(class_sel);
    printf("  Class selector specificity: %u %s\n", specificity, (specificity == 100) ? "PASS" : "FAIL");
    
    // Test ID selector
    css_selector_t *id_sel = css_selector_create(CSS_SELECTOR_TYPE_ID, "header");
    specificity = css_selector_calculate_specificity(id_sel);
    printf("  ID selector specificity: %u %s\n", specificity, (specificity == 1000) ? "PASS" : "FAIL");
    
    // Test compound selector
    element_sel->next = class_sel;
    class_sel->next = id_sel;
    specificity = css_selector_calculate_specificity(element_sel);
    printf("  Compound selector specificity: %u %s\n", specificity, (specificity == 1110) ? "PASS" : "FAIL");
    
    css_selector_destroy(element_sel);
    printf("CSS selectors test completed\n\n");
}

static void test_css_values(void) {
    printf("Testing CSS values...\n");
    
    // Test length parsing
    css_value_t *length = css_value_parse_length("100px");
    printf("  Length parsing: %s\n", length ? "PASS" : "FAIL");
    if (length) {
        printf("  Length value: %.2f %s %s\n", length->data.dimension.value, 
               length->data.dimension.unit ? length->data.dimension.unit : "NULL",
               (length->data.dimension.value == 100.0 && 
                strcmp(length->data.dimension.unit, "px") == 0) ? "PASS" : "FAIL");
        css_value_destroy(length);
    }
    
    // Test color parsing
    css_value_t *color = css_value_parse_color("#FF6600");
    printf("  Color parsing: %s\n", color ? "PASS" : "FAIL");
    if (color) {
        printf("  Color value: RGB(%d,%d,%d,%d) %s\n",
               color->data.color.r, color->data.color.g, color->data.color.b, color->data.color.a,
               (color->data.color.r == 255 && color->data.color.g == 102 && 
                color->data.color.b == 0 && color->data.color.a == 255) ? "PASS" : "FAIL");
        css_value_destroy(color);
    }
    
    // Test string parsing
    css_value_t *string = css_value_parse_string("Hello, World!");
    printf("  String parsing: %s\n", string ? "PASS" : "FAIL");
    if (string) {
        printf("  String value: \"%s\" %s\n", string->data.string,
               (strcmp(string->data.string, "Hello, World!") == 0) ? "PASS" : "FAIL");
        css_value_destroy(string);
    }
    
    printf("CSS values test completed\n\n");
}

static void test_layout_context(void) {
    printf("Testing layout context...\n");
    
    layout_context_t *ctx = layout_context_create(800, 600, 1.0);
    printf("  Layout context creation: %s\n", ctx ? "PASS" : "FAIL");
    
    if (ctx) {
        printf("  Viewport: %.0fx%.0f %s\n", ctx->viewport_width, ctx->viewport_height,
               (ctx->viewport_width == 800 && ctx->viewport_height == 600) ? "PASS" : "FAIL");
        
        printf("  DPI scale: %.1f %s\n", ctx->dpi_scale, (ctx->dpi_scale == 1.0) ? "PASS" : "FAIL");
        
        // Test viewport change
        layout_context_set_viewport(ctx, 1024, 768);
        printf("  Viewport change: %s\n", 
               (ctx->viewport_width == 1024 && ctx->viewport_height == 768) ? "PASS" : "FAIL");
        
        layout_context_destroy(ctx);
        printf("  Layout context destruction: PASS\n");
    }
    
    printf("Layout context test completed\n\n");
}

static void test_layout_box(void) {
    printf("Testing layout box...\n");
    
    // Create a simple element
    dom_element_t *element = dom_element_create("div");
    layout_box_t *box = layout_box_create(LAYOUT_BOX_BLOCK, &element->base);
    printf("  Layout box creation: %s\n", box ? "PASS" : "FAIL");
    
    if (box) {
        // Set geometry
        box->x = 100.0;
        box->y = 50.0;
        box->width = 200.0;
        box->height = 100.0;
        
        printf("  Geometry setting: PASS\n");
        printf("  Box position: (%.1f, %.1f) %s\n", box->x, box->y,
               (box->x == 100.0 && box->y == 50.0) ? "PASS" : "FAIL");
        printf("  Box size: %.1fx%.1f %s\n", box->width, box->height,
               (box->width == 200.0 && box->height == 100.0) ? "PASS" : "FAIL");
        
        // Test margins
        box->margin_top = 10.0;
        box->margin_right = 15.0;
        box->margin_bottom = 10.0;
        box->margin_left = 15.0;
        
        printf("  Margin setting: PASS\n");
        
        // Test children
        dom_element_t *child_element = dom_element_create("p");
        layout_box_t *child_box = layout_box_create(LAYOUT_BOX_BLOCK, &child_element->base);
        if (child_box) {
            layout_box_add_child(box, child_box);
            
            int child_count = 0;
            for (layout_box_t *child = box->first_child; child; child = child->next_sibling) {
                child_count++;
            }
            printf("  Child addition: %s\n", (child_count == 1) ? "PASS" : "FAIL");
            
            layout_box_destroy(child_box);
        }
        
        layout_box_destroy(box);
        printf("  Layout box destruction: PASS\n");
    }
    
    dom_element_destroy(element);
    printf("Layout box test completed\n\n");
}

static void test_html_utilities(void) {
    printf("Testing HTML utilities...\n");
    
    // Test void element detection
    bool is_void = html_is_void_element("img");
    printf("  Void element detection (img): %s\n", is_void ? "PASS" : "FAIL");
    
    is_void = html_is_void_element("div");
    printf("  Non-void element detection (div): %s\n", !is_void ? "PASS" : "FAIL");
    
    // Test tag name normalization
    char *normalized = html_normalize_tag_name("DIV");
    printf("  Tag normalization (DIV -> div): %s\n", 
           (normalized && strcmp(normalized, "div") == 0) ? "PASS" : "FAIL");
    if (normalized) free(normalized);
    
    // Test semantic element detection
    bool is_sectioning = html_is_sectioning_content("section");
    printf("  Sectioning content detection (section): %s\n", is_sectioning ? "PASS" : "FAIL");
    
    bool is_heading = html_is_heading_content("h1");
    printf("  Heading content detection (h1): %s\n", is_heading ? "PASS" : "FAIL");
    
    bool is_phasing = html_is_phrasing_content("span");
    printf("  Phrasing content detection (span): %s\n", is_phasing ? "PASS" : "FAIL");
    
    printf("HTML utilities test completed\n\n");
}

int main(void) {
    printf("=== HTML/CSS Test Suite ===\n\n");
    
    test_html_parsing();
    test_dom_manipulation();
    test_css_parsing();
    test_css_selectors();
    test_css_values();
    test_layout_context();
    test_layout_box();
    test_html_utilities();
    
    printf("=== All HTML/CSS tests completed! ===\n");
    return 0;
}
