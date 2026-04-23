#include <errno.h>

// Thread-local errno
__thread int errno_value = 0;

int *__errno_location(void) {
    return &errno_value;
}
