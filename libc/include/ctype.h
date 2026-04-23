#ifndef CTYPE_H
#define CTYPE_H

// Character classification functions
int isalnum(int c);
int isalpha(int c);
int isascii(int c);
int isblank(int c);
int iscntrl(int c);
int isdigit(int c);
int isgraph(int c);
int islower(int c);
int isprint(int c);
int ispunct(int c);
int isspace(int c);
int isupper(int c);
int isxdigit(int c);

// Character conversion functions
int tolower(int c);
int toupper(int c);

// Character classification macros
#define _ISalnum      (1 << 0)
#define _ISalpha      (1 << 1)
#define _ISblank      (1 << 2)
#define _IScntrl      (1 << 3)
#define _ISdigit      (1 << 4)
#define _ISgraph      (1 << 5)
#define _ISlower      (1 << 6)
#define _ISprint      (1 << 7)
#define _ISpunct      (1 << 8)
#define _isspace      (1 << 9)
#define _ISupper      (1 << 10)
#define _ISxdigit     (1 << 11)

// ASCII character ranges
#define _C_isalnum(c) (isalpha(c) || isdigit(c))
#define _C_isalpha(c) (((c) >= 'A' && (c) <= 'Z') || ((c) >= 'a' && (c) <= 'z'))
#define _C_isascii(c) ((unsigned int)(c) <= 127)
#define _C_isblank(c) ((c) == ' ' || (c) == '\t')
#define _C_iscntrl(c) (((c) >= 0 && (c) <= 31) || (c) == 127)
#define _C_isdigit(c) ((c) >= '0' && (c) <= '9')
#define _C_isgraph(c) ((c) >= 33 && (c) <= 126)
#define _C_islower(c) ((c) >= 'a' && (c) <= 'z')
#define _C_isprint(c) ((c) >= 32 && (c) <= 126)
#define _C_ispunct(c) (((c) >= 33 && (c) <= 47) || ((c) >= 58 && (c) <= 64) || \
                       ((c) >= 91 && (c) <= 96) || ((c) >= 123 && (c) <= 126))
#define _C_isspace(c) ((c) == ' ' || (c) == '\f' || (c) == '\n' || \
                       (c) == '\r' || (c) == '\t' || (c) == '\v')
#define _C_isupper(c) ((c) >= 'A' && (c) <= 'Z')
#define _C_isxdigit(c) (isdigit(c) || ((c) >= 'A' && (c) <= 'F') || \
                       ((c) >= 'a' && (c) <= 'f'))

#endif // CTYPE_H
