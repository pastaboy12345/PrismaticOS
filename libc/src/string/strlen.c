#include <string.h>

// strlen implementation
size_t strlen(const char *s) {
    const char *p = s;
    
    // Fast path: check for alignment
    while (((uintptr_t)p & 7) && *p) {
        p++;
    }
    
    // Check 8 bytes at a time
    while (1) {
        unsigned long long word = *(const unsigned long long*)p;
        
        // Check if any byte is zero
        if ((word - 0x0101010101010101ULL) & ~word & 0x8080808080808080ULL) {
            // Found zero byte, check which byte
            if ((word & 0xFF) == 0) return p - s;
            if ((word & 0xFF00) == 0) return p - s + 1;
            if ((word & 0xFF0000) == 0) return p - s + 2;
            if ((word & 0xFF000000) == 0) return p - s + 3;
            if ((word & 0xFF00000000ULL) == 0) return p - s + 4;
            if ((word & 0xFF0000000000ULL) == 0) return p - s + 5;
            if ((word & 0xFF000000000000ULL) == 0) return p - s + 6;
            if ((word & 0xFF00000000000000ULL) == 0) return p - s + 7;
        }
        
        p += 8;
    }
}

// strcpy implementation
char *strcpy(char *dest, const char *src) {
    char *d = dest;
    
    while ((*d++ = *src++)) {
        // Copy character
    }
    
    return dest;
}

// strncpy implementation
char *strncpy(char *dest, const char *src, size_t n) {
    char *d = dest;
    
    while (n-- && (*d++ = *src++)) {
        // Copy character
    }
    
    // Pad with zeros if necessary
    while (n--) {
        *d++ = '\0';
    }
    
    return dest;
}

// strcat implementation
char *strcat(char *dest, const char *src) {
    char *d = dest;
    
    // Find end of dest
    while (*d) {
        d++;
    }
    
    // Copy src
    while ((*d++ = *src++)) {
        // Copy character
    }
    
    return dest;
}

// strncat implementation
char *strncat(char *dest, const char *src, size_t n) {
    char *d = dest;
    
    // Find end of dest
    while (*d) {
        d++;
    }
    
    // Copy up to n characters from src
    while (n-- && *src) {
        *d++ = *src++;
    }
    
    *d = '\0';
    
    return dest;
}

// strcmp implementation
int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// strncmp implementation
int strncmp(const char *s1, const char *s2, size_t n) {
    while (n-- && *s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    
    if (n == SIZE_MAX) {
        return 0;
    }
    
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// strchr implementation
char *strchr(const char *s, int c) {
    unsigned char ch = c;
    
    while (*s) {
        if (*s == ch) {
            return (char*)s;
        }
        s++;
    }
    
    return NULL;
}

// strrchr implementation
char *strrchr(const char *s, int c) {
    const char *last = NULL;
    unsigned char ch = c;
    
    while (*s) {
        if (*s == ch) {
            last = s;
        }
        s++;
    }
    
    return (char*)last;
}

// strstr implementation
char *strstr(const char *haystack, const char *needle) {
    if (!*needle) {
        return (char*)haystack;
    }
    
    size_t needle_len = strlen(needle);
    
    while (*haystack) {
        if (*haystack == *needle && strncmp(haystack, needle, needle_len) == 0) {
            return (char*)haystack;
        }
        haystack++;
    }
    
    return NULL;
}

// strpbrk implementation
char *strpbrk(const char *s, const char *accept) {
    while (*s) {
        const char *a = accept;
        while (*a) {
            if (*s == *a) {
                return (char*)s;
            }
            a++;
        }
        s++;
    }
    
    return NULL;
}

// strcspn implementation
size_t strcspn(const char *s, const char *reject) {
    size_t count = 0;
    
    while (*s) {
        const char *r = reject;
        while (*r) {
            if (*s == *r) {
                return count;
            }
            r++;
        }
        s++;
        count++;
    }
    
    return count;
}

// strspn implementation
size_t strspn(const char *s, const char *accept) {
    size_t count = 0;
    
    while (*s) {
        const char *a = accept;
        int found = 0;
        while (*a) {
            if (*s == *a) {
                found = 1;
                break;
            }
            a++;
        }
        if (!found) {
            break;
        }
        s++;
        count++;
    }
    
    return count;
}

// strtok implementation
static char *strtok_last = NULL;

char *strtok(char *str, const char *delim) {
    return strtok_r(str, delim, &strtok_last);
}

// strtok_r implementation
char *strtok_r(char *str, const char *delim, char **saveptr) {
    if (!saveptr) return NULL;
    
    if (str) {
        *saveptr = str;
    }
    
    if (!*saveptr || !**saveptr) {
        return NULL;
    }
    
    // Skip leading delimiters
    char *token = *saveptr;
    while (*token && strcspn(token, delim) == 0) {
        token++;
    }
    
    if (!*token) {
        *saveptr = NULL;
        return NULL;
    }
    
    // Find end of token
    char *end = token + strcspn(token, delim);
    if (*end) {
        *end = '\0';
        *saveptr = end + 1;
    } else {
        *saveptr = NULL;
    }
    
    return token;
}

// strnlen implementation
size_t strnlen(const char *s, size_t maxlen) {
    size_t len = 0;
    
    while (len < maxlen && *s) {
        len++;
        s++;
    }
    
    return len;
}

// strdup implementation
char *strdup(const char *s) {
    size_t len = strlen(s) + 1;
    char *dup = malloc(len);
    
    if (dup) {
        memcpy(dup, s, len);
    }
    
    return dup;
}

// strndup implementation
char *strndup(const char *s, size_t n) {
    size_t len = strnlen(s, n) + 1;
    char *dup = malloc(len);
    
    if (dup) {
        memcpy(dup, s, len - 1);
        dup[len - 1] = '\0';
    }
    
    return dup;
}
