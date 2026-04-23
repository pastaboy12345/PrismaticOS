#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Additional type definitions
typedef uint32_t socklen_t;
typedef uint32_t nfds_t;
typedef int32_t ssize_t;
typedef int32_t mode_t;
typedef int64_t off_t;

// Forward declarations
struct sigaction;
struct sockaddr;
struct pollfd;

// Stat structure
struct stat {
    uint32_t st_dev;
    uint32_t st_ino;
    uint32_t st_mode;
    uint32_t st_nlink;
    uint32_t st_uid;
    uint32_t st_gid;
    uint32_t st_rdev;
    uint64_t st_size;
    uint64_t st_blksize;
    uint64_t st_blocks;
    uint64_t st_atime;
    uint64_t st_mtime;
    uint64_t st_ctime;
};

// Time structures
struct timeval {
    uint64_t tv_sec;
    uint64_t tv_usec;
};

// File descriptor set
typedef struct { uint32_t fds[32]; } fd_set;

// System call numbers
typedef enum {
    SYS_READ = 0,
    SYS_WRITE = 1,
    SYS_OPEN = 2,
    SYS_CLOSE = 3,
    SYS_LSEEK = 4,
    SYS_STAT = 5,
    SYS_FORK = 6,
    SYS_EXECVE = 7,
    SYS_WAITPID = 8,
    SYS_EXIT = 9,
    SYS_MMAP = 10,
    SYS_MUNMAP = 11,
    SYS_BRK = 12,
    SYS_SIGNAL = 13,
    SYS_SIGACTION = 14,
    SYS_KILL = 15,
    SYS_PTHREAD_CREATE = 16,
    SYS_PTHREAD_JOIN = 17,
    SYS_PTHREAD_MUTEX_INIT = 18,
    SYS_PTHREAD_MUTEX_LOCK = 19,
    SYS_PTHREAD_MUTEX_UNLOCK = 20,
    SYS_SOCKET = 21,
    SYS_BIND = 22,
    SYS_LISTEN = 23,
    SYS_ACCEPT = 24,
    SYS_CONNECT = 25,
    SYS_SEND = 26,
    SYS_RECV = 27,
    SYS_FCNTL = 28,
    SYS_IOCTL = 29,
    SYS_SELECT = 30,
    SYS_POLL = 31,
    SYS_MPROTECT = 32,
    SYS_GETUID = 33,
    SYS_GETGID = 34,
    SYS_SETUID = 35,
    SYS_SETGID = 36
} syscall_number_t;

// Process structure
typedef struct process {
    uint32_t pid;
    uint32_t ppid;
    uint8_t state;  // 0=running, 1=sleeping, 2=terminated
    uint64_t rsp;
    uint64_t rip;
    uint64_t cr3;
    void *heap_start;
    void *heap_end;
    void *stack_start;
    void *stack_end;
    struct process *next;
} process_t;


// Kernel functions
void kernel_main(void);
void panic(const char *message);

// Process management
process_t *process_create(void);
void process_destroy(process_t *proc);
process_t *process_get_current(void);
void process_switch(process_t *next);
int syscall_fork(void);
int syscall_execve(const char *path, char *const argv[], char *const envp[]);
int syscall_waitpid(uint32_t pid, int *status, int options);
void syscall_exit(int status);

// Memory management
void *syscall_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int syscall_munmap(void *addr, size_t length);
void *syscall_brk(void *addr);
int syscall_mprotect(void *addr, size_t len, int prot);

// File system
int syscall_open(const char *pathname, int flags, mode_t mode);
int syscall_close(int fd);
ssize_t syscall_read(int fd, void *buf, size_t count);
ssize_t syscall_write(int fd, const void *buf, size_t count);
off_t syscall_lseek(int fd, off_t offset, int whence);
int syscall_stat(const char *pathname, struct stat *statbuf);

// Signal handling
void syscall_signal(int signum, void (*handler)(int));
int syscall_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
int syscall_kill(uint32_t pid, int signum);

// Threading
int syscall_pthread_create(uint32_t *thread, void *(*start_routine)(void *), void *arg);
int syscall_pthread_join(uint32_t thread, void **retval);
int syscall_pthread_mutex_init(uint32_t *mutex);
int syscall_pthread_mutex_lock(uint32_t mutex);
int syscall_pthread_mutex_unlock(uint32_t mutex);

// Socket API
int syscall_socket(int domain, int type, int protocol);
int syscall_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int syscall_listen(int sockfd, int backlog);
int syscall_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int syscall_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
ssize_t syscall_send(int sockfd, const void *buf, size_t len, int flags);
ssize_t syscall_recv(int sockfd, void *buf, size_t len, int flags);

// Extended file operations
int syscall_fcntl(int fd, int cmd, ...);
int syscall_ioctl(int fd, unsigned long request, ...);
int syscall_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
int syscall_poll(struct pollfd *fds, nfds_t nfds, int timeout);

// User/group management
uint32_t syscall_getuid(void);
uint32_t syscall_getgid(void);
int syscall_setuid(uint32_t uid);
int syscall_setgid(uint32_t gid);

#endif // KERNEL_H
