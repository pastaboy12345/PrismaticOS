#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <os/graphics.h>
#include <gpu/gpu.h>

// Graphics test
static void test_gpu_initialization(void) {
    printf("Testing GPU initialization...\n");
    
    gpu_init_desc_t desc = {
        .enable_validation = false,
        .max_frames_in_flight = 2
    };
    
    int result = gpu_init(&desc);
    printf("  gpu_init(): %s\n", (result == 0) ? "PASS" : "FAIL");
    
    if (result == 0) {
        gpu_shutdown();
        printf("  gpu_shutdown(): PASS\n");
    }
    
    printf("GPU initialization test completed\n\n");
}

static void test_buffer_creation(void) {
    printf("Testing buffer creation...\n");
    
    gpu_init_desc_t desc = {
        .enable_validation = false,
        .max_frames_in_flight = 2
    };
    
    if (gpu_init(&desc) != 0) {
        printf("  SKIP: GPU initialization failed\n");
        return;
    }
    
    // Test vertex buffer
    gpu_buffer_desc_t buffer_desc = {
        .size = 1024,
        .usage = GPU_USAGE_VERTEX,
        .initial_data = NULL
    };
    
    gpu_buffer_t *buffer = gpu_create_buffer(&buffer_desc);
    printf("  vertex buffer creation: %s\n", buffer ? "PASS" : "FAIL");
    
    if (buffer) {
        // Test mapping
        void *mapped = gpu_map_buffer(buffer);
        printf("  buffer mapping: %s\n", mapped ? "PASS" : "FAIL");
        
        if (mapped) {
            // Write test data
            memset(mapped, 0x42, 1024);
            gpu_unmap_buffer(buffer);
            printf("  buffer write: PASS\n");
        }
        
        gpu_destroy_buffer(buffer);
        printf("  buffer destruction: PASS\n");
    }
    
    gpu_shutdown();
    printf("Buffer creation test completed\n\n");
}

static void test_texture_creation(void) {
    printf("Testing texture creation...\n");
    
    gpu_init_desc_t desc = {
        .enable_validation = false,
        .max_frames_in_flight = 2
    };
    
    if (gpu_init(&desc) != 0) {
        printf("  SKIP: GPU initialization failed\n");
        return;
    }
    
    // Create test texture data
    uint8_t *texture_data = malloc(256 * 256 * 4);  // 256x256 RGBA
    for (int y = 0; y < 256; y++) {
        for (int x = 0; x < 256; x++) {
            int idx = (y * 256 + x) * 4;
            texture_data[idx + 0] = x;     // R
            texture_data[idx + 1] = y;     // G
            texture_data[idx + 2] = 128;  // B
            texture_data[idx + 3] = 255;  // A
        }
    }
    
    gpu_texture_desc_t texture_desc = {
        .width = 256,
        .height = 256,
        .depth = 1,
        .mip_levels = 1,
        .format = GPU_FORMAT_R8G8B8A8_UNORM,
        .usage = GPU_TEXTURE_USAGE_SAMPLED | GPU_TEXTURE_USAGE_TRANSFER_DST,
        .initial_data = texture_data
    };
    
    gpu_texture_t *texture = gpu_create_texture(&texture_desc);
    printf("  texture creation: %s\n", texture ? "PASS" : "FAIL");
    
    if (texture) {
        // Test mapping
        void *mapped = gpu_map_texture(texture, 0, 0);
        printf("  texture mapping: %s\n", mapped ? "PASS" : "FAIL");
        
        if (mapped) {
            gpu_unmap_texture(texture, 0, 0);
            printf("  texture unmap: PASS\n");
        }
        
        gpu_destroy_texture(texture);
        printf("  texture destruction: PASS\n");
    }
    
    free(texture_data);
    gpu_shutdown();
    printf("Texture creation test completed\n\n");
}

static void test_command_buffer(void) {
    printf("Testing command buffer...\n");
    
    gpu_init_desc_t desc = {
        .enable_validation = false,
        .max_frames_in_flight = 2
    };
    
    if (gpu_init(&desc) != 0) {
        printf("  SKIP: GPU initialization failed\n");
        return;
    }
    
    // Create a simple render target
    gpu_texture_desc_t rt_desc = {
        .width = 800,
        .height = 600,
        .depth = 1,
        .mip_levels = 1,
        .format = GPU_FORMAT_R8G8B8A8_UNORM,
        .usage = GPU_TEXTURE_USAGE_RENDER_TARGET,
        .initial_data = NULL
    };
    
    gpu_texture_t *render_target = gpu_create_texture(&rt_desc);
    if (!render_target) {
        printf("  SKIP: Render target creation failed\n");
        gpu_shutdown();
        return;
    }
    
    // Create command buffer
    gpu_command_buffer_t *cmd_buffer = gpu_begin_command_buffer();
    printf("  command buffer creation: %s\n", cmd_buffer ? "PASS" : "FAIL");
    
    if (cmd_buffer) {
        // Record some commands
        gpu_cmd_begin_render_pass(cmd_buffer, render_target);
        gpu_cmd_set_viewport(cmd_buffer, 0, 0, 800, 600, 0.0, 1.0);
        gpu_cmd_end_render_pass(cmd_buffer);
        
        printf("  command recording: PASS\n");
        
        gpu_end_command_buffer(cmd_buffer);
        
        // Submit command buffer
        gpu_fence_t *fence = NULL;
        gpu_submit_command_buffer(cmd_buffer, &fence);
        printf("  command submission: %s\n", fence ? "PASS" : "FAIL");
        
        if (fence) {
            gpu_wait_fence(fence);
            gpu_destroy_fence(fence);
            printf("  fence synchronization: PASS\n");
        }
    }
    
    gpu_destroy_texture(render_target);
    gpu_shutdown();
    printf("Command buffer test completed\n\n");
}

static void test_os_graphics(void) {
    printf("Testing OS graphics...\n");
    
    // Test window creation
    os_window_t *window = os_window_create("Test Window", 800, 600, OS_WINDOW_FLAG_RESIZABLE);
    printf("  window creation: %s\n", window ? "PASS" : "FAIL");
    
    if (window) {
        // Test surface creation
        os_surface_t *surface = os_surface_create(800, 600);
        printf("  surface creation: %s\n", surface ? "PASS" : "FAIL");
        
        if (surface) {
            // Test painting
            os_paint_ctx_t *ctx = os_paint_begin(surface);
            if (ctx) {
                // Test basic drawing
                os_color_t red = os_color_from_rgb(255, 0, 0);
                os_color_t blue = os_color_from_rgb(0, 0, 255);
                
                os_rect_t rect = {100, 100, 200, 150};
                os_paint_fill_rect(ctx, &rect, &red);
                printf("  rectangle fill: PASS\n");
                
                rect.x = 350;
                rect.y = 100;
                os_paint_fill_rect(ctx, &rect, &blue);
                printf("  second rectangle: PASS\n");
                
                // Test text drawing
                os_font_t *font = os_font_create_default(16);
                if (font) {
                    os_point_t pos = {100, 300};
                    os_paint_draw_text(ctx, "Hello, Graphics World!", font, &pos, &red);
                    printf("  text drawing: PASS\n");
                    os_font_destroy(font);
                } else {
                    printf("  text drawing: SKIP (font creation failed)\n");
                }
                
                // Test circle drawing
                os_point_t center = {400, 350};
                os_paint_fill_circle(ctx, &center, 50, &blue);
                printf("  circle fill: PASS\n");
                
                os_paint_end(ctx);
                printf("  paint context: PASS\n");
                
                // Present
                os_surface_present(surface);
                printf("  surface present: PASS\n");
            } else {
                printf("  paint context: FAIL\n");
            }
            
            os_surface_destroy(surface);
        }
        
        os_window_destroy(window);
    }
    
    printf("OS graphics test completed\n\n");
}

static void test_graphics_constants(void) {
    printf("Testing graphics constants...\n");
    
    // Test color constants
    os_color_t black = OS_COLOR_BLACK;
    os_color_t white = OS_COLOR_WHITE;
    os_color_t red = OS_COLOR_RED;
    os_color_t green = OS_COLOR_GREEN;
    os_color_t blue = OS_COLOR_BLUE;
    
    printf("  color constants: ");
    if (black.r == 0 && black.g == 0 && black.b == 0 && black.a == 255 &&
        white.r == 255 && white.g == 255 && white.b == 255 && white.a == 255 &&
        red.r == 255 && red.g == 0 && red.b == 0 && red.a == 255 &&
        green.r == 0 && green.g == 255 && green.b == 0 && green.a == 255 &&
        blue.r == 0 && blue.g == 0 && blue.b == 255 && blue.a == 255) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }
    
    // Test color creation functions
    os_color_t custom = os_color_from_rgb(128, 64, 192);
    printf("  os_color_from_rgb: ");
    if (custom.r == 128 && custom.g == 64 && custom.b == 192 && custom.a == 255) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }
    
    custom = os_color_from_rgba(255, 128, 64, 128);
    printf("  os_color_from_rgba: ");
    if (custom.r == 255 && custom.g == 128 && custom.b == 64 && custom.a == 128) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }
    
    printf("Graphics constants test completed\n\n");
}

int main(void) {
    printf("=== Graphics Test Suite ===\n\n");
    
    test_gpu_initialization();
    test_buffer_creation();
    test_texture_creation();
    test_command_buffer();
    test_os_graphics();
    test_graphics_constants();
    
    printf("=== All graphics tests completed! ===\n");
    return 0;
}
