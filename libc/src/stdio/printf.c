#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// Simple printf implementation

// Buffer for formatted output
struct printf_buffer {
    char *buffer;
    size_t size;
    size_t pos;
    FILE *stream;
};

// Buffer operations
static int buffer_putc(struct printf_buffer *buf, char c) {
    if (buf->stream) {
        // Write to file stream
        if (fwrite(&c, 1, 1, buf->stream) != 1) {
            return -1;
        }
    } else {
        // Write to memory buffer
        if (buf->pos < buf->size) {
            buf->buffer[buf->pos++] = c;
        }
    }
    return 0;
}

static int buffer_puts(struct printf_buffer *buf, const char *s) {
    int count = 0;
    while (*s) {
        if (buffer_putc(buf, *s++) < 0) {
            return -1;
        }
        count++;
    }
    return count;
}

// Format integer
static int format_int(struct printf_buffer *buf, long long value, int base, int width, int precision, int flags) {
    char buffer[64];
    char *ptr = buffer + sizeof(buffer);
    int negative = 0;
    int count = 0;
    
    // Handle sign
    if (value < 0 && base == 10) {
        negative = 1;
        value = -value;
    }
    
    // Convert to string
    if (value == 0) {
        *--ptr = '0';
        count++;
    } else {
        while (value > 0) {
            int digit = value % base;
            *--ptr = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
            value /= base;
            count++;
        }
    }
    
    // Handle sign flag
    if (negative) {
        *--ptr = '-';
        count++;
    } else if (flags & 1) {  // '+' flag
        *--ptr = '+';
        count++;
    } else if (flags & 2) {  // ' ' flag
        *--ptr = ' ';
        count++;
    }
    
    // Handle precision
    while (count < precision) {
        *--ptr = '0';
        count++;
    }
    
    // Handle width
    while (count < width) {
        if (flags & 4) {  // '-' flag (left align)
            buffer_putc(buf, *ptr++);
        } else {
            buffer_putc(buf, ' ');
        }
        count++;
    }
    
    // Output the number
    while (*ptr) {
        buffer_putc(buf, *ptr++);
    }
    
    return 0;
}

// Format string
static int format_string(struct printf_buffer *buf, const char *s, int width, int precision, int flags) {
    int len = strlen(s);
    int count = 0;
    
    // Handle precision
    if (precision >= 0 && len > precision) {
        len = precision;
    }
    
    // Handle width (right align by default)
    while (count < width - len) {
        buffer_putc(buf, ' ');
        count++;
    }
    
    // Output string
    for (int i = 0; i < len; i++) {
        buffer_putc(buf, s[i]);
    }
    
    return 0;
}

// Main printf formatting function
static int vprintf_internal(struct printf_buffer *buf, const char *format, va_list args) {
    int count = 0;
    
    while (*format) {
        if (*format == '%') {
            format++;
            
            // Parse flags
            int flags = 0;
            while (*format) {
                if (*format == '+') flags |= 1;
                else if (*format == ' ') flags |= 2;
                else if (*format == '-') flags |= 4;
                else if (*format == '0') flags |= 8;
                else break;
                format++;
            }
            
            // Parse width
            int width = 0;
            if (*format == '*') {
                width = va_arg(args, int);
                format++;
            } else {
                while (*format >= '0' && *format <= '9') {
                    width = width * 10 + (*format - '0');
                    format++;
                }
            }
            
            // Parse precision
            int precision = -1;
            if (*format == '.') {
                format++;
                if (*format == '*') {
                    precision = va_arg(args, int);
                    format++;
                } else {
                    precision = 0;
                    while (*format >= '0' && *format <= '9') {
                        precision = precision * 10 + (*format - '0');
                        format++;
                    }
                }
            }
            
            // Parse length modifier
            int length = 0;
            if (*format == 'h') {
                if (format[1] == 'h') {
                    length = 1;  // hh
                    format += 2;
                } else {
                    length = 2;  // h
                    format++;
                }
            } else if (*format == 'l') {
                if (format[1] == 'l') {
                    length = 4;  // ll
                    format += 2;
                } else {
                    length = 3;  // l
                    format++;
                }
            } else if (*format == 'j') {
                length = 5;  // intmax_t
                format++;
            } else if (*format == 'z') {
                length = 6;  // size_t
                format++;
            } else if (*format == 't') {
                length = 7;  // ptrdiff_t
                format++;
            } else if (*format == 'L') {
                length = 8;  // long double
                format++;
            }
            
            // Parse conversion specifier
            switch (*format) {
                case '%':
                    buffer_putc(buf, '%');
                    count++;
                    break;
                    
                case 'c':
                    {
                        char c = va_arg(args, int);
                        buffer_putc(buf, c);
                        count++;
                    }
                    break;
                    
                case 's':
                    {
                        const char *s = va_arg(args, const char*);
                        if (!s) s = "(null)";
                        format_string(buf, s, width, precision, flags);
                        count += strlen(s);
                    }
                    break;
                    
                case 'd':
                case 'i':
                    {
                        long long value;
                        if (length == 1) value = (signed char)va_arg(args, int);
                        else if (length == 2) value = (short)va_arg(args, int);
                        else if (length == 3) value = va_arg(args, long);
                        else if (length == 4) value = va_arg(args, long long);
                        else value = va_arg(args, int);
                        
                        format_int(buf, value, 10, width, precision, flags);
                        count++;
                    }
                    break;
                    
                case 'u':
                    {
                        unsigned long long value;
                        if (length == 1) value = (unsigned char)va_arg(args, unsigned int);
                        else if (length == 2) value = (unsigned short)va_arg(args, unsigned int);
                        else if (length == 3) value = va_arg(args, unsigned long);
                        else if (length == 4) value = va_arg(args, unsigned long long);
                        else value = va_arg(args, unsigned int);
                        
                        format_int(buf, value, 10, width, precision, flags);
                        count++;
                    }
                    break;
                    
                case 'x':
                case 'X':
                    {
                        unsigned long long value;
                        if (length == 1) value = (unsigned char)va_arg(args, unsigned int);
                        else if (length == 2) value = (unsigned short)va_arg(args, unsigned int);
                        else if (length == 3) value = va_arg(args, unsigned long);
                        else if (length == 4) value = va_arg(args, unsigned long long);
                        else value = va_arg(args, unsigned int);
                        
                        format_int(buf, value, 16, width, precision, flags);
                        count++;
                    }
                    break;
                    
                case 'o':
                    {
                        unsigned long long value;
                        if (length == 1) value = (unsigned char)va_arg(args, unsigned int);
                        else if (length == 2) value = (unsigned short)va_arg(args, unsigned int);
                        else if (length == 3) value = va_arg(args, unsigned long);
                        else if (length == 4) value = va_arg(args, unsigned long long);
                        else value = va_arg(args, unsigned int);
                        
                        format_int(buf, value, 8, width, precision, flags);
                        count++;
                    }
                    break;
                    
                case 'p':
                    {
                        void *ptr = va_arg(args, void*);
                        buffer_puts(buf, "0x");
                        format_int(buf, (unsigned long long)ptr, 16, 0, 0, 0);
                        count += 2;
                    }
                    break;
                    
                case 'n':
                    {
                        int *n = va_arg(args, int*);
                        if (n) *n = count;
                    }
                    break;
                    
                default:
                    buffer_putc(buf, '%');
                    buffer_putc(buf, *format);
                    count += 2;
                    break;
            }
            
            format++;
        } else {
            buffer_putc(buf, *format++);
            count++;
        }
    }
    
    return count;
}

// printf implementation
int printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    struct printf_buffer buf = {
        .buffer = NULL,
        .size = 0,
        .pos = 0,
        .stream = stdout
    };
    
    int result = vprintf_internal(&buf, format, args);
    va_end(args);
    
    return result;
}

// fprintf implementation
int fprintf(FILE *stream, const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    struct printf_buffer buf = {
        .buffer = NULL,
        .size = 0,
        .pos = 0,
        .stream = stream
    };
    
    int result = vprintf_internal(&buf, format, args);
    va_end(args);
    
    return result;
}

// sprintf implementation
int sprintf(char *str, const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    struct printf_buffer buf = {
        .buffer = str,
        .size = SIZE_MAX,
        .pos = 0,
        .stream = NULL
    };
    
    int result = vprintf_internal(&buf, format, args);
    
    // Null terminate
    if (buf.pos < buf.size) {
        str[buf.pos] = '\0';
    }
    
    va_end(args);
    
    return result;
}

// snprintf implementation
int snprintf(char *str, size_t size, const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    struct printf_buffer buf = {
        .buffer = str,
        .size = size - 1,  // Leave room for null terminator
        .pos = 0,
        .stream = NULL
    };
    
    int result = vprintf_internal(&buf, format, args);
    
    // Null terminate
    if (size > 0 && buf.pos < size) {
        str[buf.pos] = '\0';
    } else if (size > 0) {
        str[size - 1] = '\0';
    }
    
    va_end(args);
    
    return result;
}
