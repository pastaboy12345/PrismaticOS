#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <html/html.h>
#include <css/css.h>
#include <layout/layout.h>
#include <render/render.h>

// Performance benchmark
typedef struct benchmark_result {
    char *name;
    uint64_t time_ms;
    size_t operations;
    double ops_per_second;
} benchmark_result_t;

typedef struct benchmark_suite {
    benchmark_result_t *results;
    size_t count;
    size_t capacity;
    uint64_t total_time;
} benchmark_suite_t;

static benchmark_suite_t *benchmark_suite_create(void) {
    benchmark_suite_t *suite = malloc(sizeof(benchmark_suite_t));
    if (!suite) return NULL;
    
    memset(suite, 0, sizeof(benchmark_suite_t));
    suite->capacity = 100;
    suite->results = malloc(suite->capacity * sizeof(benchmark_result_t));
    
    return suite;
}

static void benchmark_suite_destroy(benchmark_suite_t *suite) {
    if (!suite) return;
    
    for (size_t i = 0; i < suite->count; i++) {
        free(suite->results[i].name);
    }
    free(suite->results);
    free(suite);
}

static void benchmark_add_result(benchmark_suite_t *suite, const char *name, uint64_t time_ms, size_t operations) {
    if (!suite || !name) return;
    
    if (suite->count >= suite->capacity) {
        suite->capacity *= 2;
        suite->results = realloc(suite->results, suite->capacity * sizeof(benchmark_result_t));
    }
    
    benchmark_result_t *result = &suite->results[suite->count];
    result->name = strdup(name);
    result->time_ms = time_ms;
    result->operations = operations;
    result->ops_per_second = operations > 0 ? (operations * 1000.0) / time_ms : 0.0;
    
    suite->total_time += time_ms;
    suite->count++;
}

static uint64_t get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

static void benchmark_html_parsing(benchmark_suite_t *suite) {
    printf("Benchmarking HTML parsing...\n");
    
    // Generate test HTML
    char *html = malloc(1000000);  // 1MB test HTML
    char *ptr = html;
    
    strcpy(ptr, "<!DOCTYPE html><html><head><title>Test</title></head><body>");
    ptr += strlen(ptr);
    
    // Generate many elements
    for (int i = 0; i < 10000; i++) {
        ptr += sprintf(ptr, "<div class=\"item\" id=\"item%d\"><h2>Item %d</h2>", i, i);
        ptr += sprintf(ptr, "<p>This is item %d with some content.</p>", i);
        ptr += sprintf(ptr, "<ul>");
        for (int j = 0; j < 10; j++) {
            ptr += sprintf(ptr, "<li>List item %d-%d</li>", i, j);
        }
        ptr += sprintf(ptr, "</ul></div>");
    }
    
    strcpy(ptr, "</body></html>");
    
    // Benchmark parsing
    uint64_t start_time = get_time_ms();
    
    html_parser_t *parser = html_parser_create();
    html_document_t *doc = html_parser_parse(parser, html);
    
    uint64_t end_time = get_time_ms();
    uint64_t parsing_time = end_time - start_time;
    
    html_parser_destroy(parser);
    if (doc) html_document_destroy(doc);
    
    benchmark_add_result(suite, "HTML Parsing (1MB)", parsing_time, 1);
    benchmark_add_result(suite, "HTML Elements", parsing_time, 10000 * 12);  // ~12 elements per item
    
    free(html);
}

static void benchmark_css_parsing(benchmark_suite_t *suite) {
    printf("Benchmarking CSS parsing...\n");
    
    // Generate test CSS
    char *css = malloc(500000);  // 500KB test CSS
    char *ptr = css;
    
    strcpy(ptr, "body { font-family: Arial; margin: 0; padding: 0; } ");
    ptr += strlen(ptr);
    
    // Generate many rules
    for (int i = 0; i < 5000; i++) {
        ptr += sprintf(ptr, ".item%d { color: #%06x; background: #%06x; margin: %dpx; padding: %dpx; } ",
                      i, rand() % 0xFFFFFF, rand() % 0xFFFFFF, i % 20, i % 10);
        ptr += sprintf(ptr, "#item%d:hover { transform: scale(1.1); transition: all 0.3s; } ",
                      i);
        ptr += sprintf(ptr, "div.item%d > p { line-height: 1.%d; font-size: %dpx; } ",
                      i, i % 9 + 1, 12 + (i % 8));
    }
    
    // Benchmark parsing
    uint64_t start_time = get_time_ms();
    
    css_parser_t *parser = css_parser_create();
    css_stylesheet_t *stylesheet = css_parser_parse(parser, css);
    
    uint64_t end_time = get_time_ms();
    uint64_t parsing_time = end_time - start_time;
    
    css_parser_destroy(parser);
    if (stylesheet) css_stylesheet_destroy(stylesheet);
    
    benchmark_add_result(suite, "CSS Parsing (500KB)", parsing_time, 1);
    benchmark_add_result(suite, "CSS Rules", parsing_time, 5000 * 3);  // 3 rules per item
    
    free(css);
}

static void benchmark_layout(benchmark_suite_t *suite) {
    printf("Benchmarking layout...\n");
    
    // Create simple DOM
    html_document_t *doc = html_document_create("test://example.com");
    
    // Create elements
    dom_element_t *body = dom_element_create("body");
    dom_node_append_child(&doc->base, &body->base);
    
    // Add many elements
    for (int i = 0; i < 1000; i++) {
        dom_element_t *div = dom_element_create("div");
        dom_node_add_class(&div->base, "item");
        dom_node_set_attribute(&div->base, "id", "item");
        
        dom_element_t *h2 = dom_element_create("h2");
        dom_text_t *text = dom_text_create("Heading");
        dom_node_append_child(&h2->base, &text->base);
        dom_node_append_child(&div->base, &h2->base);
        
        dom_element_t *p = dom_element_create("p");
        text = dom_text_create("Paragraph content");
        dom_node_append_child(&p->base, &text->base);
        dom_node_append_child(&div->base, &p->base);
        
        dom_node_append_child(&body->base, &div->base);
    }
    
    // Create stylesheet
    css_stylesheet_t *stylesheet = css_stylesheet_create("test://style.css");
    
    // Benchmark layout
    layout_context_t *ctx = layout_context_create(800, 600, 1.0);
    layout_engine_t *engine = layout_engine_create(ctx);
    
    uint64_t start_time = get_time_ms();
    
    layout_box_t *root_box = layout_engine_build_layout(engine, &doc->base, stylesheet);
    
    uint64_t end_time = get_time_ms();
    uint64_t layout_time = end_time - start_time;
    
    benchmark_add_result(suite, "Layout (1000 elements)", layout_time, 1);
    benchmark_add_result(suite, "Layout Elements", layout_time, 1000 * 3);  // 3 elements per item
    
    // Cleanup
    if (root_box) layout_box_destroy(root_box);
    layout_engine_destroy(engine);
    layout_context_destroy(ctx);
    css_stylesheet_destroy(stylesheet);
    html_document_destroy(doc);
}

static void benchmark_rendering(benchmark_suite_t *suite) {
    printf("Benchmarking rendering...\n");
    
    // Create layout
    layout_context_t *ctx = layout_context_create(800, 600, 1.0);
    renderer_t *renderer = renderer_create(ctx);
    
    // Create simple layout boxes
    layout_box_t *root = layout_box_create(LAYOUT_BOX_BLOCK, NULL);
    
    // Add many boxes
    for (int i = 0; i < 1000; i++) {
        layout_box_t *box = layout_box_create(LAYOUT_BOX_BLOCK, NULL);
        box->width = 100 + (i % 200);
        box->height = 50 + (i % 100);
        box->x = (i % 8) * 100;
        box->y = (i / 8) * 60;
        
        layout_box_add_child(root, box);
    }
    
    // Benchmark rendering
    uint64_t start_time = get_time_ms();
    
    display_list_t *dl = renderer_render_layout_box(renderer, root);
    
    uint64_t end_time = get_time_ms();
    uint64_t render_time = end_time - start_time;
    
    benchmark_add_result(suite, "Rendering (1000 boxes)", render_time, 1);
    benchmark_add_result(suite, "Render Boxes", render_time, 1000);
    
    // Cleanup
    if (dl) display_list_destroy(dl);
    layout_box_destroy(root);
    renderer_destroy(renderer);
    layout_context_destroy(ctx);
}

static void benchmark_memory(benchmark_suite_t *suite) {
    printf("Benchmarking memory operations...\n");
    
    const size_t iterations = 100000;
    const size_t alloc_size = 1024;
    
    // Benchmark malloc/free
    uint64_t start_time = get_time_ms();
    
    void **ptrs = malloc(iterations * sizeof(void*));
    for (size_t i = 0; i < iterations; i++) {
        ptrs[i] = malloc(alloc_size);
    }
    for (size_t i = 0; i < iterations; i++) {
        free(ptrs[i]);
    }
    free(ptrs);
    
    uint64_t end_time = get_time_ms();
    uint64_t malloc_time = end_time - start_time;
    
    benchmark_add_result(suite, "malloc/free (100K)", malloc_time, iterations * 2);
    
    // Benchmark string operations
    start_time = get_time_ms();
    
    char **strings = malloc(iterations * sizeof(char*));
    for (size_t i = 0; i < iterations; i++) {
        strings[i] = malloc(64);
        snprintf(strings[i], 64, "string_%zu", i);
    }
    for (size_t i = 0; i < iterations; i++) {
        free(strings[i]);
    }
    free(strings);
    
    end_time = get_time_ms();
    uint64_t string_time = end_time - start_time;
    
    benchmark_add_result(suite, "String ops (100K)", string_time, iterations * 2);
}

static void benchmark_print_results(benchmark_suite_t *suite) {
    printf("\n=== Benchmark Results ===\n");
    printf("Total time: %lu ms\n\n", suite->total_time);
    
    printf("%-30s %12s %12s %15s\n", "Test", "Time (ms)", "Operations", "Ops/sec");
    printf("%-30s %12s %12s %15s\n", "----", "---------", "----------", "--------");
    
    for (size_t i = 0; i < suite->count; i++) {
        benchmark_result_t *result = &suite->results[i];
        printf("%-30s %12lu %12zu %15.0f\n", 
               result->name, result->time_ms, result->operations, result->ops_per_second);
    }
    
    printf("\nSummary:\n");
    printf("Total benchmarks: %zu\n", suite->count);
    printf("Average time per benchmark: %.2f ms\n", (double)suite->total_time / suite->count);
    
    // Find slowest and fastest
    uint64_t slowest = 0, fastest = UINT64_MAX;
    size_t slowest_idx = 0, fastest_idx = 0;
    
    for (size_t i = 0; i < suite->count; i++) {
        if (suite->results[i].time_ms > slowest) {
            slowest = suite->results[i].time_ms;
            slowest_idx = i;
        }
        if (suite->results[i].time_ms < fastest) {
            fastest = suite->results[i].time_ms;
            fastest_idx = i;
        }
    }
    
    printf("Slowest: %s (%lu ms)\n", suite->results[slowest_idx].name, slowest);
    printf("Fastest: %s (%lu ms)\n", suite->results[fastest_idx].name, fastest);
}

int main(void) {
    printf("Modern OS Performance Benchmark\n");
    printf("===============================\n\n");
    
    benchmark_suite_t *suite = benchmark_suite_create();
    if (!suite) {
        printf("Failed to create benchmark suite\n");
        return 1;
    }
    
    // Run benchmarks
    benchmark_html_parsing(suite);
    benchmark_css_parsing(suite);
    benchmark_layout(suite);
    benchmark_rendering(suite);
    benchmark_memory(suite);
    
    // Print results
    benchmark_print_results(suite);
    
    // Cleanup
    benchmark_suite_destroy(suite);
    
    printf("\nBenchmark completed successfully!\n");
    return 0;
}
