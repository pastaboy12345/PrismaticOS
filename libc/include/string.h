#ifndef STRING_H
#define STRING_H

#include <stddef.h>

// String operations
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memchr(const void *s, int c, size_t n);
void *memrchr(const void *s, int c, size_t n);

// String functions
size_t strlen(const char *s);
char *strcpy(char *dest, const char *src);
char *strncpy(char *dest, const char *src, size_t n);
char *strcat(char *dest, const char *src);
char *strncat(char *dest, const char *src, size_t n);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
int strcoll(const char *s1, const char *s2);
size_t strxfrm(char *dest, const char *src, size_t n);

// String searching
char *strchr(const char *s, int c);
char *strrchr(const char *s, int c);
char *strstr(const char *haystack, const char *needle);
char *strpbrk(const char *s, const char *accept);
size_t strcspn(const char *s, const char *reject);
size_t strspn(const char *s, const char *accept);
char *strtok(char *str, const char *delim);
char *strtok_r(char *str, const char *delim, char **saveptr);

// String to number conversion
double strtod(const char *nptr, char **endptr);
float strtof(const char *nptr, char **endptr);
long double strtold(const char *nptr, char **endptr);
long int strtol(const char *nptr, char **endptr, int base);
long long int strtoll(const char *nptr, char **endptr, int base);
unsigned long int strtoul(const char *nptr, char **endptr, int base);
unsigned long long int strtoull(const char *nptr, char **endptr, int base);

// Error messages
char *strerror(int errnum);
int strerror_r(int errnum, char *buf, size_t buflen);

// String duplication
char *strdup(const char *s);
char *strndup(const char *s, size_t n);

// String length with limit
size_t strnlen(const char *s, size_t maxlen);

// Memory string operations
void *mempcpy(void *dest, const void *src, size_t n);
void *memmem(const void *haystack, size_t haystacklen,
             const void *needle, size_t needlelen);

#endif // STRING_H
