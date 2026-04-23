#include <ctype.h>

// Character classification functions
int isalnum(int c) {
    return _C_isalnum(c);
}

int isalpha(int c) {
    return _C_isalpha(c);
}

int isascii(int c) {
    return _C_isascii(c);
}

int isblank(int c) {
    return _C_isblank(c);
}

int iscntrl(int c) {
    return _C_iscntrl(c);
}

int isdigit(int c) {
    return _C_isdigit(c);
}

int isgraph(int c) {
    return _C_isgraph(c);
}

int islower(int c) {
    return _C_islower(c);
}

int isprint(int c) {
    return _C_isprint(c);
}

int ispunct(int c) {
    return _C_ispunct(c);
}

int isspace(int c) {
    return _C_isspace(c);
}

int isupper(int c) {
    return _C_isupper(c);
}

int isxdigit(int c) {
    return _C_isxdigit(c);
}

// Character conversion functions
int tolower(int c) {
    return isupper(c) ? c + 32 : c;
}

int toupper(int c) {
    return islower(c) ? c - 32 : c;
}
