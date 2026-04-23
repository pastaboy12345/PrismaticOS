#ifndef GPU_H
#define GPU_H

#include <stdint.h>
#include <stddef.h>

// GPU resource types
typedef enum {
    GPU_RESOURCE_BUFFER,
    GPU_RESOURCE_TEXTURE,
    GPU_RESOURCE_PIPELINE,
    GPU_RESOURCE_SAMPLER,
    GPU_RESOURCE_RENDER_TARGET
} gpu_resource_type_t;

// Buffer usage flags
#define GPU_USAGE_VERTEX        0x01
#define GPU_USAGE_INDEX         0x02
#define GPU_USAGE_UNIFORM       0x04
#define GPU_USAGE_STORAGE       0x08
#define GPU_USAGE_TRANSFER_SRC  0x10
#define GPU_USAGE_TRANSFER_DST  0x20

// Texture formats
typedef enum {
    GPU_FORMAT_R8_UNORM,
    GPU_FORMAT_R8G8_UNORM,
    GPU_FORMAT_R8G8B8A8_UNORM,
    GPU_FORMAT_B8G8R8A8_UNORM,
    GPU_FORMAT_R32_SFLOAT,
    GPU_FORMAT_R32G32_SFLOAT,
    GPU_FORMAT_R32G32B32A32_SFLOAT,
    GPU_FORMAT_D32_SFLOAT
} gpu_format_t;

// Texture usage flags
#define GPU_TEXTURE_USAGE_SAMPLED      0x01
#define GPU_TEXTURE_USAGE_RENDER_TARGET 0x02
#define GPU_TEXTURE_USAGE_STORAGE      0x04
#define GPU_TEXTURE_USAGE_TRANSFER_SRC  0x08
#define GPU_TEXTURE_USAGE_TRANSFER_DST  0x10

// Shader stages
typedef enum {
    GPU_SHADER_STAGE_VERTEX,
    GPU_SHADER_STAGE_FRAGMENT,
    GPU_SHADER_STAGE_COMPUTE
} gpu_shader_stage_t;

// Blend factors
typedef enum {
    GPU_BLEND_FACTOR_ZERO,
    GPU_BLEND_FACTOR_ONE,
    GPU_BLEND_FACTOR_SRC_COLOR,
    GPU_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
    GPU_BLEND_FACTOR_DST_COLOR,
    GPU_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
    GPU_BLEND_FACTOR_SRC_ALPHA,
    GPU_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    GPU_BLEND_FACTOR_DST_ALPHA,
    GPU_BLEND_FACTOR_ONE_MINUS_DST_ALPHA
} gpu_blend_factor_t;

// Blend operations
typedef enum {
    GPU_BLEND_OP_ADD,
    GPU_BLEND_OP_SUBTRACT,
    GPU_BLEND_OP_REVERSE_SUBTRACT,
    GPU_BLEND_OP_MIN,
    GPU_BLEND_OP_MAX
} gpu_blend_op_t;

// Filter modes
typedef enum {
    GPU_FILTER_NEAREST,
    GPU_FILTER_LINEAR
} gpu_filter_t;

// Address modes
typedef enum {
    GPU_ADDRESS_MODE_REPEAT,
    GPU_ADDRESS_MODE_MIRRORED_REPEAT,
    GPU_ADDRESS_MODE_CLAMP_TO_EDGE,
    GPU_ADDRESS_MODE_CLAMP_TO_BORDER,
    GPU_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE
} gpu_address_mode_t;

// Primitive topologies
typedef enum {
    GPU_PRIMITIVE_TOPOLOGY_POINT_LIST,
    GPU_PRIMITIVE_TOPOLOGY_LINE_LIST,
    GPU_PRIMITIVE_TOPOLOGY_LINE_STRIP,
    GPU_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    GPU_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP
} gpu_primitive_topology_t;

// Front face
typedef enum {
    GPU_FRONT_FACE_COUNTER_CLOCKWISE,
    GPU_FRONT_FACE_CLOCKWISE
} gpu_front_face_t;

// Cull mode
typedef enum {
    GPU_CULL_MODE_NONE,
    GPU_CULL_MODE_FRONT,
    GPU_CULL_MODE_BACK
} gpu_cull_mode_t;

// Compare function
typedef enum {
    GPU_COMPARE_FUNC_NEVER,
    GPU_COMPARE_FUNC_LESS,
    GPU_COMPARE_FUNC_EQUAL,
    GPU_COMPARE_FUNC_LESS_EQUAL,
    GPU_COMPARE_FUNC_GREATER,
    GPU_COMPARE_FUNC_NOT_EQUAL,
    GPU_COMPARE_FUNC_GREATER_EQUAL,
    GPU_COMPARE_FUNC_ALWAYS
} gpu_compare_func_t;

// Stencil operations
typedef enum {
    GPU_STENCIL_OP_KEEP,
    GPU_STENCIL_OP_ZERO,
    GPU_STENCIL_OP_REPLACE,
    GPU_STENCIL_OP_INCREMENT_CLAMP,
    GPU_STENCIL_OP_DECREMENT_CLAMP,
    GPU_STENCIL_OP_INVERT,
    GPU_STENCIL_OP_INCREMENT_WRAP,
    GPU_STENCIL_OP_DECREMENT_WRAP
} gpu_stencil_op_t;

// GPU resource handles
typedef struct gpu_buffer gpu_buffer_t;
typedef struct gpu_texture gpu_texture_t;
typedef struct gpu_pipeline gpu_pipeline_t;
typedef struct gpu_sampler gpu_sampler_t;
typedef struct gpu_fence gpu_fence_t;
typedef struct gpu_command_buffer gpu_command_buffer_t;

// Buffer descriptor
typedef struct gpu_buffer_desc {
    size_t size;
    uint32_t usage;
    const void *initial_data;
} gpu_buffer_desc_t;

// Texture descriptor
typedef struct gpu_texture_desc {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mip_levels;
    gpu_format_t format;
    uint32_t usage;
    const void *initial_data;
} gpu_texture_desc_t;

// Shader descriptor
typedef struct gpu_shader_desc {
    gpu_shader_stage_t stage;
    const uint32_t *bytecode;
    size_t bytecode_size;
    const char *entry_point;
} gpu_shader_desc_t;

// Pipeline descriptor
typedef struct gpu_pipeline_desc {
    gpu_shader_desc_t vertex_shader;
    gpu_shader_desc_t fragment_shader;
    
    // Vertex input
    struct {
        uint32_t stride;
        uint32_t offset;
        gpu_format_t format;
    } vertex_attributes[16];
    uint32_t vertex_attribute_count;
    
    // Rasterization
    gpu_primitive_topology_t primitive_topology;
    gpu_front_face_t front_face;
    gpu_cull_mode_t cull_mode;
    
    // Depth stencil
    struct {
        bool depth_test_enable;
        bool depth_write_enable;
        gpu_compare_func_t depth_compare_func;
        bool stencil_enable;
        gpu_stencil_op_t stencil_fail_op;
        gpu_stencil_op_t stencil_depth_fail_op;
        gpu_stencil_op_t stencil_pass_op;
        gpu_compare_func_t stencil_compare_func;
    } depth_stencil;
    
    // Color blend
    struct {
        bool blend_enable;
        gpu_blend_factor_t src_factor;
        gpu_blend_factor_t dst_factor;
        gpu_blend_op_t blend_op;
        gpu_blend_factor_t src_factor_alpha;
        gpu_blend_factor_t dst_factor_alpha;
        gpu_blend_op_t blend_op_alpha;
        uint32_t write_mask;
    } color_blend_targets[8];
    uint32_t color_blend_target_count;
} gpu_pipeline_desc_t;

// Sampler descriptor
typedef struct gpu_sampler_desc {
    gpu_filter_t min_filter;
    gpu_filter_t mag_filter;
    gpu_filter_t mipmap_filter;
    gpu_address_mode_t address_mode_u;
    gpu_address_mode_t address_mode_v;
    gpu_address_mode_t address_mode_w;
    float min_lod;
    float max_lod;
    float lod_bias;
    float max_anisotropy;
} gpu_sampler_desc_t;

// Color write mask
#define GPU_COLOR_WRITE_MASK_RED   0x01
#define GPU_COLOR_WRITE_MASK_GREEN 0x02
#define GPU_COLOR_WRITE_MASK_BLUE  0x04
#define GPU_COLOR_WRITE_MASK_ALPHA 0x08
#define GPU_COLOR_WRITE_MASK_ALL   0x0F

// GPU initialization
typedef struct gpu_init_desc {
    bool enable_validation;
    uint32_t max_frames_in_flight;
} gpu_init_desc_t;

int gpu_init(const gpu_init_desc_t *desc);
void gpu_shutdown(void);
uint32_t gpu_get_frame_index(void);

// Resource creation
gpu_buffer_t *gpu_create_buffer(const gpu_buffer_desc_t *desc);
void gpu_destroy_buffer(gpu_buffer_t *buffer);
gpu_texture_t *gpu_create_texture(const gpu_texture_desc_t *desc);
void gpu_destroy_texture(gpu_texture_t *texture);
gpu_pipeline_t *gpu_create_pipeline(const gpu_pipeline_desc_t *desc);
void gpu_destroy_pipeline(gpu_pipeline_t *pipeline);
gpu_sampler_t *gpu_create_sampler(const gpu_sampler_desc_t *desc);
void gpu_destroy_sampler(gpu_sampler_t *sampler);

// Resource mapping
void *gpu_map_buffer(gpu_buffer_t *buffer);
void gpu_unmap_buffer(gpu_buffer_t *buffer);
void *gpu_map_texture(gpu_texture_t *texture, uint32_t level, uint32_t array_slice);
void gpu_unmap_texture(gpu_texture_t *texture, uint32_t level, uint32_t array_slice);

// Command buffer management
gpu_command_buffer_t *gpu_begin_command_buffer(void);
void gpu_end_command_buffer(gpu_command_buffer_t *cmd_buffer);
void gpu_submit_command_buffer(gpu_command_buffer_t *cmd_buffer, gpu_fence_t **signal_fence);
void gpu_wait_fence(gpu_fence_t *fence);
void gpu_destroy_fence(gpu_fence_t *fence);

// Command recording
void gpu_cmd_begin_render_pass(gpu_command_buffer_t *cmd_buffer, gpu_texture_t *render_target);
void gpu_cmd_end_render_pass(gpu_command_buffer_t *cmd_buffer);
void gpu_cmd_set_pipeline(gpu_command_buffer_t *cmd_buffer, gpu_pipeline_t *pipeline);
void gpu_cmd_set_vertex_buffer(gpu_command_buffer_t *cmd_buffer, uint32_t slot, gpu_buffer_t *buffer, uint64_t offset);
void gpu_cmd_set_index_buffer(gpu_command_buffer_t *cmd_buffer, gpu_buffer_t *buffer, uint64_t offset);
void gpu_cmd_set_viewport(gpu_command_buffer_t *cmd_buffer, float x, float y, float width, float height, float min_depth, float max_depth);
void gpu_cmd_set_scissor(gpu_command_buffer_t *cmd_buffer, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void gpu_cmd_bind_descriptor_set(gpu_command_buffer_t *cmd_buffer, uint32_t set, void *descriptor_set);
void gpu_cmd_draw(gpu_command_buffer_t *cmd_buffer, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance);
void gpu_cmd_draw_indexed(gpu_command_buffer_t *cmd_buffer, uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance);
void gpu_cmd_copy_buffer(gpu_command_buffer_t *cmd_buffer, gpu_buffer_t *src, gpu_buffer_t *dst, uint64_t src_offset, uint64_t dst_offset, uint64_t size);
void gpu_cmd_copy_texture(gpu_command_buffer_t *cmd_buffer, gpu_texture_t *src, gpu_texture_t *dst, uint32_t src_level, uint32_t dst_level);
void gpu_cmd_barrier(gpu_command_buffer_t *cmd_buffer);

// Present
void gpu_present(gpu_texture_t *texture);

#endif // GPU_H
