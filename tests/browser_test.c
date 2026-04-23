#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <html/html.h>
#include <css/css.h>
#include <layout/layout.h>
#include <render/render.h>
#include <os/graphics.h>
#include <js/js.h>

// Simple browser test application
typedef struct browser_test {
    html_document_t *document;
    css_stylesheet_t *stylesheet;
    layout_context_t *layout_ctx;
    renderer_t *renderer;
    js_runtime_t *js_runtime;
    os_window_t *window;
    os_surface_t *surface;
} browser_test_t;

static browser_test_t *browser_test_create(void) {
    browser_test_t *browser = malloc(sizeof(browser_test_t));
    if (!browser) return NULL;
    
    memset(browser, 0, sizeof(browser_test_t));
    
    // Create HTML document
    browser->document = html_document_create("test://example.com");
    if (!browser->document) {
        free(browser);
        return NULL;
    }
    
    // Create CSS stylesheet
    browser->stylesheet = css_stylesheet_create("test://example.com/style.css");
    if (!browser->stylesheet) {
        html_document_destroy(browser->document);
        free(browser);
        return NULL;
    }
    
    // Create layout context
    browser->layout_ctx = layout_context_create(800, 600, 1.0);
    if (!browser->layout_ctx) {
        css_stylesheet_destroy(browser->stylesheet);
        html_document_destroy(browser->document);
        free(browser);
        return NULL;
    }
    
    // Create renderer
    browser->renderer = renderer_create(browser->layout_ctx);
    if (!browser->renderer) {
        layout_context_destroy(browser->layout_ctx);
        css_stylesheet_destroy(browser->stylesheet);
        html_document_destroy(browser->document);
        free(browser);
        return NULL;
    }
    
    // Create JavaScript runtime
    browser->js_runtime = js_runtime_create(NULL);
    if (!browser->js_runtime) {
        renderer_destroy(browser->renderer);
        layout_context_destroy(browser->layout_ctx);
        css_stylesheet_destroy(browser->stylesheet);
        html_document_destroy(browser->document);
        free(browser);
        return NULL;
    }
    
    // Create window
    browser->window = os_window_create("Browser Test", 800, 600, OS_WINDOW_FLAG_RESIZABLE);
    if (!browser->window) {
        js_runtime_destroy(browser->js_runtime);
        renderer_destroy(browser->renderer);
        layout_context_destroy(browser->layout_ctx);
        css_stylesheet_destroy(browser->stylesheet);
        html_document_destroy(browser->document);
        free(browser);
        return NULL;
    }
    
    // Create surface
    browser->surface = os_surface_create(800, 600);
    if (!browser->surface) {
        os_window_destroy(browser->window);
        js_runtime_destroy(browser->js_runtime);
        renderer_destroy(browser->renderer);
        layout_context_destroy(browser->layout_ctx);
        css_stylesheet_destroy(browser->stylesheet);
        html_document_destroy(browser->document);
        free(browser);
        return NULL;
    }
    
    // Bind surface to window
    os_window_bind_surface(browser->window, browser->surface);
    
    return browser;
}

static void browser_test_destroy(browser_test_t *browser) {
    if (!browser) return;
    
    if (browser->surface) os_surface_destroy(browser->surface);
    if (browser->window) os_window_destroy(browser->window);
    if (browser->js_runtime) js_runtime_destroy(browser->js_runtime);
    if (browser->renderer) renderer_destroy(browser->renderer);
    if (browser->layout_ctx) layout_context_destroy(browser->layout_ctx);
    if (browser->stylesheet) css_stylesheet_destroy(browser->stylesheet);
    if (browser->document) html_document_destroy(browser->document);
    
    free(browser);
}

static bool browser_test_load_html(browser_test_t *browser, const char *html) {
    if (!browser || !html) return false;
    
    // Parse HTML
    html_parser_t *parser = html_parser_create();
    if (!parser) return false;
    
    html_document_t *new_doc = html_parser_parse(parser, html);
    html_parser_destroy(parser);
    
    if (!new_doc) return false;
    
    // Replace old document
    html_document_destroy(browser->document);
    browser->document = new_doc;
    
    // Bind DOM to JavaScript
    js_bind_dom(browser->js_runtime, &new_doc->base);
    
    return true;
}

static bool browser_test_load_css(browser_test_t *browser, const char *css) {
    if (!browser || !css) return false;
    
    // Parse CSS
    css_parser_t *parser = css_parser_create();
    if (!parser) return false;
    
    css_stylesheet_t *new_stylesheet = css_parser_parse(parser, css);
    css_parser_destroy(parser);
    
    if (!new_stylesheet) return false;
    
    // Replace old stylesheet
    css_stylesheet_destroy(browser->stylesheet);
    browser->stylesheet = new_stylesheet;
    
    return true;
}

static bool browser_test_render(browser_test_t *browser) {
    if (!browser) return false;
    
    // Build layout
    layout_engine_t *layout_engine = layout_engine_create(browser->layout_ctx);
    if (!layout_engine) return false;
    
    layout_box_t *root_box = layout_engine_build_layout(layout_engine, &browser->document->base, browser->stylesheet);
    layout_engine_destroy(layout_engine);
    
    if (!root_box) return false;
    
    // Render to surface
    renderer_set_main_target(browser->renderer, render_target_create(800, 600));
    renderer_begin_frame(browser->renderer);
    renderer_render(browser->renderer);
    renderer_end_frame(browser->renderer);
    
    // Present
    os_surface_present(browser->surface);
    
    return true;
}

static void browser_test_run_javascript(browser_test_t *browser, const char *script) {
    if (!browser || !script) return;
    
    js_runtime_enter_context(browser->js_runtime);
    js_value_t *result = js_runtime_execute(browser->js_runtime, script);
    
    if (js_runtime_has_exception(browser->js_runtime)) {
        js_error_t *error = js_runtime_get_exception(browser->js_runtime);
        printf("JavaScript Error: %s\n", error ? error->message : "Unknown error");
        if (error) js_error_destroy(error);
    } else if (result) {
        char *result_str = js_value_to_string(result);
        printf("JavaScript Result: %s\n", result_str);
        free(result_str);
        js_value_destroy(result);
    }
    
    js_runtime_exit_context(browser->js_runtime);
}

static void browser_test_run(void) {
    printf("=== Browser Test ===\n");
    
    // Create browser
    browser_test_t *browser = browser_test_create();
    if (!browser) {
        printf("Failed to create browser\n");
        return;
    }
    
    // Load HTML
    const char *html = 
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head>\n"
        "    <title>Test Page</title>\n"
        "    <meta charset=\"UTF-8\">\n"
        "</head>\n"
        "<body>\n"
        "    <h1>Hello World</h1>\n"
        "    <p>This is a test page for the browser engine.</p>\n"
        "    <div id=\"content\">\n"
        "        <p>Content area</p>\n"
        "        <button onclick=\"alert('Hello!')\">Click me</button>\n"
        "    </div>\n"
        "    <script>\n"
        "        console.log('Page loaded');\n"
        "        document.getElementById('content').style.color = 'blue';\n"
        "    </script>\n"
        "</body>\n"
        "</html>";
    
    if (!browser_test_load_html(browser, html)) {
        printf("Failed to load HTML\n");
        browser_test_destroy(browser);
        return;
    }
    
    printf("HTML loaded successfully\n");
    
    // Load CSS
    const char *css = 
        "body {\n"
        "    font-family: Arial, sans-serif;\n"
        "    margin: 20px;\n"
        "    background-color: #f0f0f0;\n"
        "}\n"
        "h1 {\n"
        "    color: #333;\n"
        "    text-align: center;\n"
        "}\n"
        "p {\n"
        "    line-height: 1.6;\n"
        "}\n"
        "#content {\n"
        "    border: 1px solid #ccc;\n"
        "    padding: 10px;\n"
        "    margin: 10px 0;\n"
        "}\n"
        "button {\n"
        "    background-color: #007bff;\n"
        "    color: white;\n"
        "    border: none;\n"
        "    padding: 10px 20px;\n"
        "    cursor: pointer;\n"
        "}\n"
        "button:hover {\n"
        "    background-color: #0056b3;\n"
        "}";
    
    if (!browser_test_load_css(browser, css)) {
        printf("Failed to load CSS\n");
        browser_test_destroy(browser);
        return;
    }
    
    printf("CSS loaded successfully\n");
    
    // Render
    if (!browser_test_render(browser)) {
        printf("Failed to render\n");
        browser_test_destroy(browser);
        return;
    }
    
    printf("Rendered successfully\n");
    
    // Run JavaScript
    printf("\n--- Running JavaScript ---\n");
    browser_test_run_javascript(browser, "document.title");
    browser_test_run_javascript(browser, "document.getElementById('content').textContent");
    browser_test_run_javascript(browser, "document.querySelectorAll('p').length");
    browser_test_run_javascript(browser, "window.getComputedStyle(document.body).backgroundColor");
    
    // Show window
    os_window_show(browser->window);
    printf("Window displayed\n");
    
    // Simple event loop (simplified)
    printf("Press Enter to exit...\n");
    getchar();
    
    // Cleanup
    browser_test_destroy(browser);
    printf("Browser test completed\n");
}

int main(void) {
    printf("Modern OS Browser Test\n");
    printf("====================\n\n");
    
    browser_test_run();
    
    return 0;
}
