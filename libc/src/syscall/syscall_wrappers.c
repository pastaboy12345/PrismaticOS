#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <errno.h>
#include "syscall.h"

// System call wrappers for libc

// File operations
int close(int fd) {
    long result = syscall_close(fd);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

ssize_t read(int fd, void *buf, size_t count) {
    long result = syscall_read(fd, buf, count);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

ssize_t write(int fd, const void *buf, size_t count) {
    long result = syscall_write(fd, buf, count);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

int open(const char *pathname, int flags, ...) {
    mode_t mode = 0;
    
    if (flags & O_CREAT) {
        va_list args;
        va_start(args, flags);
        mode = va_arg(args, mode_t);
        va_end(args);
    }
    
    long result = syscall_open(pathname, flags, mode);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

off_t lseek(int fd, off_t offset, int whence) {
    long result = syscall_lseek(fd, offset, whence);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

int stat(const char *pathname, struct stat *statbuf) {
    long result = syscall_stat(pathname, statbuf);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

int fstat(int fd, struct stat *statbuf) {
    // For simplicity, use stat with /proc/self/fd
    char path[64];
    snprintf(path, sizeof(path), "/proc/self/fd/%d", fd);
    return stat(path, statbuf);
}

int fcntl(int fd, int cmd, ...) {
    va_list args;
    va_start(args, cmd);
    long arg = va_arg(args, long);
    va_end(args);
    
    long result = syscall(SYS_fcntl, fd, cmd, arg);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

int ioctl(int fd, unsigned long request, ...) {
    va_list args;
    va_start(args, request);
    long arg = va_arg(args, long);
    va_end(args);
    
    long result = syscall(SYS_ioctl, fd, request, arg);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

// Process operations
pid_t fork(void) {
    long result = syscall(SYS_fork);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

int execve(const char *pathname, char *const argv[], char *const envp[]) {
    long result = syscall(SYS_execve, pathname, argv, envp);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

void exit(int status) {
    syscall(SYS_exit, status);
    // Should never return
}

pid_t waitpid(pid_t pid, int *status, int options) {
    long result = syscall(SYS_waitpid, pid, status, options);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

pid_t getpid(void) {
    // For simplicity, return a hardcoded value
    // In a real implementation, this would be a system call
    return 1;
}

pid_t getppid(void) {
    // For simplicity, return a hardcoded value
    // In a real implementation, this would be a system call
    return 0;
}

uid_t getuid(void) {
    long result = syscall(SYS_getuid);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

gid_t getgid(void) {
    long result = syscall(SYS_getgid);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

int setuid(uid_t uid) {
    long result = syscall(SYS_setuid, uid);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

int setgid(gid_t gid) {
    long result = syscall(SYS_setgid, gid);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

// Memory operations
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    long result = syscall(SYS_mmap, addr, length, prot, flags, fd, offset);
    if (result < 0) {
        errno = -result;
        return MAP_FAILED;
    }
    return (void*)result;
}

int munmap(void *addr, size_t length) {
    long result = syscall(SYS_munmap, addr, length);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

void *sbrk(ptrdiff_t increment) {
    long result = syscall(SYS_brk, 0);
    if (result < 0) {
        errno = -result;
        return (void*)-1;
    }
    
    void *old_brk = (void*)result;
    void *new_brk = (void*)((char*)old_brk + increment);
    
    result = syscall(SYS_brk, new_brk);
    if (result < 0) {
        errno = -result;
        return (void*)-1;
    }
    
    return old_brk;
}

int brk(void *addr) {
    long result = syscall(SYS_brk, addr);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

int mprotect(void *addr, size_t len, int prot) {
    long result = syscall(SYS_mprotect, addr, len, prot);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

// Signal handling
void (*signal(int signum, void (*handler)(int)))(int) {
    struct sigaction act, oldact;
    act.sa_handler = handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    
    if (sigaction(signum, &act, &oldact) < 0) {
        return SIG_ERR;
    }
    
    return oldact.sa_handler;
}

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
    long result = syscall(SYS_sigaction, signum, act, oldact);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

int kill(pid_t pid, int signum) {
    long result = syscall(SYS_kill, pid, signum);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

// Socket operations
int socket(int domain, int type, int protocol) {
    long result = syscall(SYS_socket, domain, type, protocol);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    long result = syscall(SYS_bind, sockfd, addr, addrlen);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

int listen(int sockfd, int backlog) {
    long result = syscall(SYS_listen, sockfd, backlog);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    long result = syscall(SYS_accept, sockfd, addr, addrlen);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    long result = syscall(SYS_connect, sockfd, addr, addrlen);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    long result = syscall(SYS_send, sockfd, buf, len, flags);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    long result = syscall(SYS_recv, sockfd, buf, len, flags);
    if (result < 0) {
        errno = -result;
        return -1;
    }
    return result;
}

// Directory operations
int mkdir(const char *pathname, mode_t mode) {
    // For simplicity, use open with O_CREAT and check if it's a directory
    int fd = open(pathname, O_CREAT | O_EXCL | O_DIRECTORY, mode);
    if (fd < 0) {
        return -1;
    }
    close(fd);
    return 0;
}

int rmdir(const char *pathname) {
    // For simplicity, use unlink
    return unlink(pathname);
}

int unlink(const char *pathname) {
    // This would be a system call in a real implementation
    // For now, return success
    return 0;
}

int rename(const char *oldpath, const char *newpath) {
    // This would be a system call in a real implementation
    // For now, return success
    return 0;
}

int chdir(const char *path) {
    // This would be a system call in a real implementation
    // For now, return success
    return 0;
}

char *getcwd(char *buf, size_t size) {
    // For simplicity, return a hardcoded path
    if (buf && size > 1) {
        strcpy(buf, "/");
        return buf;
    }
    return NULL;
}

// Environment variables
char *getenv(const char *name) {
    // For simplicity, return NULL for all environment variables
    // In a real implementation, this would access the environment
    return NULL;
}

int setenv(const char *name, const char *value, int overwrite) {
    // For simplicity, return success
    // In a real implementation, this would modify the environment
    return 0;
}

int unsetenv(const char *name) {
    // For simplicity, return success
    // In a real implementation, this would modify the environment
    return 0;
}
