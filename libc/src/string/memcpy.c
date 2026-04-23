#include <string.h>

// Optimized memcpy implementation
void *memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    
    // Handle small copies
    if (n < 8) {
        while (n--) {
            *d++ = *s++;
        }
        return dest;
    }
    
    // Align destination to 8 bytes
    while (((uintptr_t)d & 7) && n) {
        *d++ = *s++;
        n--;
    }
    
    // Copy 8 bytes at a time
    while (n >= 8) {
        *(unsigned long long*)d = *(const unsigned long long*)s;
        d += 8;
        s += 8;
        n -= 8;
    }
    
    // Copy remaining bytes
    while (n--) {
        *d++ = *s++;
    }
    
    return dest;
}

// memmove implementation
void *memmove(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    
    // Check for overlap
    if (d <= s || d >= s + n) {
        // No overlap, use memcpy
        return memcpy(dest, src, n);
    }
    
    // Overlap - copy backwards
    d += n - 1;
    s += n - 1;
    
    while (n--) {
        *d-- = *s--;
    }
    
    return dest;
}

// memset implementation
void *memset(void *s, int c, size_t n) {
    unsigned char *p = s;
    unsigned char value = c;
    
    // Align to 8 bytes
    while (((uintptr_t)p & 7) && n) {
        *p++ = value;
        n--;
    }
    
    // Set 8 bytes at a time
    unsigned long long value64 = ((unsigned long long)value << 56) |
                                ((unsigned long long)value << 48) |
                                ((unsigned long long)value << 40) |
                                ((unsigned long long)value << 32) |
                                ((unsigned long long)value << 24) |
                                ((unsigned long long)value << 16) |
                                ((unsigned long long)value << 8) |
                                value;
    
    while (n >= 8) {
        *(unsigned long long*)p = value64;
        p += 8;
        n -= 8;
    }
    
    // Set remaining bytes
    while (n--) {
        *p++ = value;
    }
    
    return s;
}

// memcmp implementation
int memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *p1 = s1;
    const unsigned char *p2 = s2;
    
    // Compare 8 bytes at a time
    while (n >= 8) {
        unsigned long long v1 = *(const unsigned long long*)p1;
        unsigned long long v2 = *(const unsigned long long*)p2;
        
        if (v1 != v2) {
            // Compare byte by byte to find the difference
            for (int i = 0; i < 8; i++) {
                unsigned char c1 = p1[i];
                unsigned char c2 = p2[i];
                if (c1 != c2) {
                    return c1 - c2;
                }
            }
        }
        
        p1 += 8;
        p2 += 8;
        n -= 8;
    }
    
    // Compare remaining bytes
    while (n--) {
        unsigned char c1 = *p1++;
        unsigned char c2 = *p2++;
        if (c1 != c2) {
            return c1 - c2;
        }
    }
    
    return 0;
}

// memchr implementation
void *memchr(const void *s, int c, size_t n) {
    const unsigned char *p = s;
    unsigned char value = c;
    
    while (n--) {
        if (*p == value) {
            return (void*)p;
        }
        p++;
    }
    
    return NULL;
}

// memrchr implementation
void *memrchr(const void *s, int c, size_t n) {
    const unsigned char *p = (const unsigned char*)s + n - 1;
    unsigned char value = c;
    
    while (n--) {
        if (*p == value) {
            return (void*)p;
        }
        p--;
    }
    
    return NULL;
}
