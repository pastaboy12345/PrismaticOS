#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Memory allocation implementation based on jemalloc concepts

// Size classes for small allocations
#define NUM_SIZE_CLASSES 32
#define MIN_SIZE_CLASS 8
#define MAX_SMALL_SIZE 4096

// Arena structure
typedef struct arena {
    struct arena *next;
    void *chunks;
    size_t chunk_size;
    size_t used;
} arena_t;

// Size class structure
typedef struct size_class {
    size_t size;
    void **free_list;
    size_t free_count;
    arena_t *arena;
} size_class_t;

// Block header for small allocations
typedef struct block_header {
    size_t size;
    size_t magic;
} block_header_t;

#define BLOCK_MAGIC 0xDEADBEEF

// Global allocator state
static arena_t *main_arena = NULL;
static size_class_t size_classes[NUM_SIZE_CLASSES];
static int allocator_initialized = 0;

// Initialize size classes
static void init_size_classes(void) {
    size_t size = MIN_SIZE_CLASS;
    
    for (int i = 0; i < NUM_SIZE_CLASSES; i++) {
        size_classes[i].size = size;
        size_classes[i].free_list = NULL;
        size_classes[i].free_count = 0;
        size_classes[i].arena = main_arena;
        
        if (size < MAX_SMALL_SIZE) {
            size *= 2;
        } else {
            size += MAX_SMALL_SIZE;
        }
    }
}

// Initialize allocator
static void init_allocator(void) {
    if (allocator_initialized) return;
    
    // Create main arena
    main_arena = sbrk(sizeof(arena_t));
    if (main_arena == (void*)-1) {
        return;  // Failed to initialize
    }
    
    main_arena->next = NULL;
    main_arena->chunks = NULL;
    main_arena->chunk_size = 0;
    main_arena->used = 0;
    
    // Initialize size classes
    init_size_classes();
    
    allocator_initialized = 1;
}

// Find appropriate size class
static int find_size_class(size_t size) {
    for (int i = 0; i < NUM_SIZE_CLASSES; i++) {
        if (size <= size_classes[i].size) {
            return i;
        }
    }
    return -1;  // Too large for size classes
}

// Allocate from size class
static void *alloc_from_size_class(int class_idx) {
    size_class_t *sc = &size_classes[class_idx];
    
    if (sc->free_list && sc->free_count > 0) {
        // Use existing free block
        void *ptr = sc->free_list[sc->free_count - 1];
        sc->free_count--;
        return ptr;
    }
    
    // Allocate new chunk if needed
    if (!sc->arena->chunks || sc->arena->used >= sc->arena->chunk_size) {
        size_t chunk_size = 64 * 1024;  // 64KB chunks
        void *chunk = sbrk(chunk_size);
        if (chunk == (void*)-1) {
            return NULL;
        }
        
        sc->arena->chunks = chunk;
        sc->arena->chunk_size = chunk_size;
        sc->arena->used = 0;
    }
    
    // Allocate block from chunk
    void *ptr = (char*)sc->arena->chunks + sc->arena->used;
    sc->arena->used += sc->size;
    
    return ptr;
}

// Free to size class
static void free_to_size_class(void *ptr, int class_idx) {
    size_class_t *sc = &size_classes[class_idx];
    
    // Simple free list implementation
    if (sc->free_count < 1024) {  // Limit free list size
        // Expand free list if needed
        if (sc->free_count % 64 == 0) {
            void **new_list = realloc(sc->free_list, (sc->free_count + 64) * sizeof(void*));
            if (!new_list) return;
            sc->free_list = new_list;
        }
        
        sc->free_list[sc->free_count++] = ptr;
    }
}

// Large allocation (direct mmap)
static void *alloc_large(size_t size) {
    // Align size to page boundary
    size = (size + 4095) & ~4095;
    
    void *ptr = sbrk(size);
    if (ptr == (void*)-1) {
        return NULL;
    }
    
    block_header_t *header = (block_header_t*)ptr;
    header->size = size;
    header->magic = BLOCK_MAGIC;
    
    return (void*)((char*)ptr + sizeof(block_header_t));
}

// Free large allocation
static void free_large(void *ptr) {
    block_header_t *header = (block_header_t*)((char*)ptr - sizeof(block_header_t));
    
    if (header->magic != BLOCK_MAGIC) {
        return;  // Invalid header
    }
    
    // For simplicity, we don't actually free large allocations
    // In a real implementation, this would use munmap
}

// malloc implementation
void *malloc(size_t size) {
    if (size == 0) return NULL;
    
    init_allocator();
    
    if (size <= MAX_SMALL_SIZE) {
        int class_idx = find_size_class(size);
        if (class_idx >= 0) {
            return alloc_from_size_class(class_idx);
        }
    }
    
    return alloc_large(size);
}

// calloc implementation
void *calloc(size_t nmemb, size_t size) {
    if (nmemb == 0 || size == 0) return NULL;
    
    // Check for overflow
    if (nmemb > SIZE_MAX / size) {
        return NULL;
    }
    
    size_t total_size = nmemb * size;
    void *ptr = malloc(total_size);
    if (ptr) {
        memset(ptr, 0, total_size);
    }
    
    return ptr;
}

// realloc implementation
void *realloc(void *ptr, size_t new_size) {
    if (!ptr) return malloc(new_size);
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }
    
    // Get old size (simplified - in real implementation we'd store this)
    block_header_t *header = (block_header_t*)((char*)ptr - sizeof(block_header_t));
    size_t old_size = 0;
    
    if (header->magic == BLOCK_MAGIC) {
        old_size = header->size - sizeof(block_header_t);
    } else {
        // Small allocation - estimate size
        int class_idx = find_size_class(old_size);
        if (class_idx >= 0) {
            old_size = size_classes[class_idx].size;
        }
    }
    
    if (new_size <= old_size) {
        return ptr;  // Current block is large enough
    }
    
    // Allocate new block and copy data
    void *new_ptr = malloc(new_size);
    if (!new_ptr) return NULL;
    
    size_t copy_size = old_size < new_size ? old_size : new_size;
    memcpy(new_ptr, ptr, copy_size);
    free(ptr);
    
    return new_ptr;
}

// free implementation
void free(void *ptr) {
    if (!ptr) return;
    
    // Check if it's a large allocation
    block_header_t *header = (block_header_t*)((char*)ptr - sizeof(block_header_t));
    
    if (header->magic == BLOCK_MAGIC) {
        free_large(ptr);
        return;
    }
    
    // Find appropriate size class
    int class_idx = find_size_class(1);  // Start with smallest
    for (int i = 0; i < NUM_SIZE_CLASSES; i++) {
        if (ptr >= size_classes[i].arena->chunks && 
            ptr < (char*)size_classes[i].arena->chunks + size_classes[i].arena->chunk_size) {
            class_idx = i;
            break;
        }
    }
    
    if (class_idx >= 0) {
        free_to_size_class(ptr, class_idx);
    }
}
