#ifndef STDIO_H
#define STDIO_H

#include <stddef.h>
#include <stdarg.h>

// File types
#define FILE_TYPE_READ   1
#define FILE_TYPE_WRITE  2
#define FILE_TYPE_APPEND 4

// File structure
typedef struct {
    int fd;
    int flags;
    unsigned char *buffer;
    size_t buffer_size;
    size_t buffer_pos;
    size_t buffer_used;
    int eof;
    int error;
} FILE;

// Standard file streams
extern FILE *stdin;
extern FILE *stdout;
extern FILE *stderr;

// File operations
FILE *fopen(const char *pathname, const char *mode);
FILE *fdopen(int fd, const char *mode);
int fclose(FILE *stream);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);
void rewind(FILE *stream);
int fgetc(FILE *stream);
int fputc(int c, FILE *stream);
char *fgets(char *s, int size, FILE *stream);
int fputs(const char *s, FILE *stream);
int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);
int scanf(const char *format, ...);
int fscanf(FILE *stream, const char *format, ...);
int sscanf(const char *str, const char *format, ...);

// Error handling
void clearerr(FILE *stream);
int feof(FILE *stream);
int ferror(FILE *stream);
void perror(const char *s);

// File positioning
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#endif // STDIO_H
