#include <stdlib.h>
#include <ctype.h>

// Convert string to integer
int atoi(const char *nptr) {
    return (int)strtol(nptr, NULL, 10);
}

// Convert string to long
long atol(const char *nptr) {
    return strtol(nptr, NULL, 10);
}

// Convert string to long long
long long atoll(const char *nptr) {
    return strtoll(nptr, NULL, 10);
}

// Convert string to double
double atof(const char *nptr) {
    return strtod(nptr, NULL);
}

// Convert string to long with error checking
long strtol(const char *nptr, char **endptr, int base) {
    const char *s = nptr;
    unsigned long acc;
    int c;
    unsigned long cutoff;
    int neg = 0, any, cutlim;
    
    // Skip leading whitespace
    do {
        c = *s++;
    } while (isspace(c));
    
    // Handle sign
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+') {
        c = *s++;
    }
    
    // Handle base prefix
    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    } else if (base == 0) {
        base = c == '0' ? 8 : 10;
    }
    
    // Validate base
    if (base < 2 || base > 36) {
        if (endptr) *endptr = (char *)nptr;
        return 0;
    }
    
    cutoff = neg ? -(unsigned long)LONG_MIN : LONG_MAX;
    cutlim = cutoff % base;
    cutoff /= base;
    
    for (acc = 0, any = 0; ; c = *s++) {
        if (isdigit(c)) {
            c -= '0';
        } else if (isalpha(c)) {
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        } else {
            break;
        }
        
        if (c >= base) {
            break;
        }
        
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
            any = -1;
        } else {
            any = 1;
            acc *= base;
            acc += c;
        }
    }
    
    if (any < 0) {
        acc = neg ? LONG_MIN : LONG_MAX;
    } else if (neg) {
        acc = -acc;
    }
    
    if (endptr) {
        *endptr = any ? (char *)s - 1 : (char *)nptr;
    }
    
    return acc;
}

// Convert string to long long
long long strtoll(const char *nptr, char **endptr, int base) {
    const char *s = nptr;
    unsigned long long acc;
    int c;
    unsigned long long cutoff;
    int neg = 0, any, cutlim;
    
    // Skip leading whitespace
    do {
        c = *s++;
    } while (isspace(c));
    
    // Handle sign
    if (c == '-') {
        neg = 1;
        c = *s++;
    } else if (c == '+') {
        c = *s++;
    }
    
    // Handle base prefix
    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    } else if (base == 0) {
        base = c == '0' ? 8 : 10;
    }
    
    // Validate base
    if (base < 2 || base > 36) {
        if (endptr) *endptr = (char *)nptr;
        return 0;
    }
    
    cutoff = neg ? -(unsigned long long)LLONG_MIN : LLONG_MAX;
    cutlim = cutoff % base;
    cutoff /= base;
    
    for (acc = 0, any = 0; ; c = *s++) {
        if (isdigit(c)) {
            c -= '0';
        } else if (isalpha(c)) {
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        } else {
            break;
        }
        
        if (c >= base) {
            break;
        }
        
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
            any = -1;
        } else {
            any = 1;
            acc *= base;
            acc += c;
        }
    }
    
    if (any < 0) {
        acc = neg ? LLONG_MIN : LLONG_MAX;
    } else if (neg) {
        acc = -acc;
    }
    
    if (endptr) {
        *endptr = any ? (char *)s - 1 : (char *)nptr;
    }
    
    return acc;
}

// Convert string to unsigned long
unsigned long strtoul(const char *nptr, char **endptr, int base) {
    const char *s = nptr;
    unsigned long acc;
    int c;
    unsigned long cutoff;
    int any, cutlim;
    
    // Skip leading whitespace
    do {
        c = *s++;
    } while (isspace(c));
    
    // Handle sign
    if (c == '-') {
        c = *s++;
    } else if (c == '+') {
        c = *s++;
    }
    
    // Handle base prefix
    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    } else if (base == 0) {
        base = c == '0' ? 8 : 10;
    }
    
    // Validate base
    if (base < 2 || base > 36) {
        if (endptr) *endptr = (char *)nptr;
        return 0;
    }
    
    cutoff = ULONG_MAX / base;
    cutlim = ULONG_MAX % base;
    
    for (acc = 0, any = 0; ; c = *s++) {
        if (isdigit(c)) {
            c -= '0';
        } else if (isalpha(c)) {
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        } else {
            break;
        }
        
        if (c >= base) {
            break;
        }
        
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
            any = -1;
        } else {
            any = 1;
            acc *= base;
            acc += c;
        }
    }
    
    if (any < 0) {
        acc = ULONG_MAX;
    }
    
    if (endptr) {
        *endptr = any ? (char *)s - 1 : (char *)nptr;
    }
    
    return acc;
}

// Convert string to unsigned long long
unsigned long long strtoull(const char *nptr, char **endptr, int base) {
    const char *s = nptr;
    unsigned long long acc;
    int c;
    unsigned long long cutoff;
    int any, cutlim;
    
    // Skip leading whitespace
    do {
        c = *s++;
    } while (isspace(c));
    
    // Handle sign
    if (c == '-') {
        c = *s++;
    } else if (c == '+') {
        c = *s++;
    }
    
    // Handle base prefix
    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    } else if (base == 0) {
        base = c == '0' ? 8 : 10;
    }
    
    // Validate base
    if (base < 2 || base > 36) {
        if (endptr) *endptr = (char *)nptr;
        return 0;
    }
    
    cutoff = ULLONG_MAX / base;
    cutlim = ULLONG_MAX % base;
    
    for (acc = 0, any = 0; ; c = *s++) {
        if (isdigit(c)) {
            c -= '0';
        } else if (isalpha(c)) {
            c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        } else {
            break;
        }
        
        if (c >= base) {
            break;
        }
        
        if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
            any = -1;
        } else {
            any = 1;
            acc *= base;
            acc += c;
        }
    }
    
    if (any < 0) {
        acc = ULLONG_MAX;
    }
    
    if (endptr) {
        *endptr = any ? (char *)s - 1 : (char *)nptr;
    }
    
    return acc;
}
