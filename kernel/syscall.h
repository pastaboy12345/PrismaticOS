#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>
#include <stddef.h>

// Type definitions
typedef int32_t pid_t;
typedef uint32_t uid_t;
typedef uint32_t gid_t;
typedef uint32_t pthread_t;
typedef struct { uint32_t value; } pthread_mutex_t;
typedef struct { uint32_t value; } pthread_mutexattr_t;
typedef struct { uint32_t value; } pthread_attr_t;
#include "kernel.h"

// System call wrapper macros
#define SYSCALL0(num) \
    ({ \
        long ret; \
        asm volatile("syscall" : "=a"(ret) : "a"(num) : "rcx", "r11", "memory"); \
        ret; \
    })

#define SYSCALL1(num, arg1) \
    ({ \
        long ret; \
        asm volatile("syscall" : "=a"(ret) : "a"(num), "D"(arg1) : "rcx", "r11", "memory"); \
        ret; \
    })

#define SYSCALL2(num, arg1, arg2) \
    ({ \
        long ret; \
        asm volatile("syscall" : "=a"(ret) : "a"(num), "D"(arg1), "S"(arg2) : "rcx", "r11", "memory"); \
        ret; \
    })

#define SYSCALL3(num, arg1, arg2, arg3) \
    ({ \
        long ret; \
        asm volatile("syscall" : "=a"(ret) : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3) : "rcx", "r11", "memory"); \
        ret; \
    })

#define SYSCALL4(num, arg1, arg2, arg3, arg4) \
    ({ \
        long ret; \
        register long r10 asm("r10") = (long)(arg4); \
        asm volatile("syscall" : "=a"(ret) : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3), "r"(r10) : "rcx", "r11", "memory"); \
        ret; \
    })

#define SYSCALL5(num, arg1, arg2, arg3, arg4, arg5) \
    ({ \
        long ret; \
        register long r10 asm("r10") = (long)(arg4); \
        register long r8 asm("r8") = (long)(arg5); \
        asm volatile("syscall" : "=a"(ret) : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3), "r"(r10), "r"(r8) : "rcx", "r11", "memory"); \
        ret; \
    })

#define SYSCALL6(num, arg1, arg2, arg3, arg4, arg5, arg6) \
    ({ \
        long ret; \
        register long r10 asm("r10") = (long)(arg4); \
        register long r8 asm("r8") = (long)(arg5); \
        register long r9 asm("r9") = (long)(arg6); \
        asm volatile("syscall" : "=a"(ret) : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3), "r"(r10), "r"(r8), "r"(r9) : "rcx", "r11", "memory"); \
        ret; \
    })

// Initialize syscall system
void syscall_init(void);

// System call implementations (user-space wrappers)
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
int open(const char *pathname, int flags, mode_t mode);
int close(int fd);
off_t lseek(int fd, off_t offset, int whence);
int stat(const char *pathname, struct stat *statbuf);
pid_t fork(void);
int execve(const char *path, char *const argv[], char *const envp[]);
pid_t waitpid(pid_t pid, int *status, int options);
void exit(int status);
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);
void *brk(void *addr);
void *sbrk(ptrdiff_t increment);
void (*signal(int signum, void (*handler)(int)))(int);
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
int kill(pid_t pid, int signum);
int pthread_create(pthread_t *thread, const pthread_attr_t *attr, 
                  void *(*start_routine)(void *), void *arg);
int pthread_join(pthread_t thread, void **retval);
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
int socket(int domain, int type, int protocol);
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int listen(int sockfd, int backlog);
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
int fcntl(int fd, int cmd, ...);
int ioctl(int fd, unsigned long request, ...);
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
int mprotect(void *addr, size_t len, int prot);
uid_t getuid(void);
gid_t getgid(void);
int setuid(uid_t uid);
int setgid(gid_t gid);

#endif // SYSCALL_H
