// Simple 32-bit memory management
#include "kernel.h"

// Simple string functions
void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t*)s;
    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }
    return s;
}

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t*)dest;
    const uint8_t *s = (const uint8_t*)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

char *strncpy(char *dest, const char *src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

char *strstr(const char *haystack, const char *needle) {
    if (!needle || !*needle) return (char*)haystack;
    
    for (; *haystack; haystack++) {
        const char *h = haystack;
        const char *n = needle;
        
        while (*n && *h && *h == *n) {
            h++;
            n++;
        }
        
        if (!*n) return (char*)haystack;
    }
    
    return NULL;
}

// Simple kernel heap
static uint8_t *kernel_heap = (uint8_t*)0x1000000;  // 16MB mark
static size_t heap_size = 0;
static const size_t max_heap_size = 0x1000000;  // 16MB

void mm_init(void) {
    heap_size = 0;
}

size_t mm_get_available_memory(void) {
    return max_heap_size - heap_size;
}

void *kmalloc(size_t size) {
    if (heap_size + size > max_heap_size) {
        return NULL;
    }
    
    void *ptr = kernel_heap + heap_size;
    heap_size += size;
    return ptr;
}

void kfree(void *ptr) {
    // Simple implementation - don't actually free
    (void)ptr;
}

void *kcalloc(size_t nmemb, size_t size) {
    size_t total = nmemb * size;
    void *ptr = kmalloc(total);
    if (ptr) {
        memset(ptr, 0, total);
    }
    return ptr;
}

void *krealloc(void *ptr, size_t size) {
    void *new_ptr = kmalloc(size);
    if (new_ptr && ptr) {
        // Simple copy - not tracking original size
        memcpy(new_ptr, ptr, size);
    }
    return new_ptr;
}
