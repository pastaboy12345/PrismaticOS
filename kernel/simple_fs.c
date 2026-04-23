// Simple 32-bit filesystem
#include "kernel.h"
#include <string.h>

void fs_init(void) {
    // Simple filesystem init - do nothing
}

int syscall_open(const char *pathname, int flags, mode_t mode) {
    (void)pathname; (void)flags; (void)mode;
    return -1; // Not implemented
}

int syscall_close(int fd) {
    (void)fd;
    return -1; // Not implemented
}

ssize_t syscall_read(int fd, void *buf, size_t count) {
    (void)fd; (void)buf; (void)count;
    return -1; // Not implemented
}

ssize_t syscall_write(int fd, const void *buf, size_t count) {
    (void)fd; (void)buf; (void)count;
    return -1; // Not implemented
}

off_t syscall_lseek(int fd, off_t offset, int whence) {
    (void)fd; (void)offset; (void)whence;
    return -1; // Not implemented
}
