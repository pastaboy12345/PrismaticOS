#include "syscall.h"
#include "kernel.h"
#include "process.h"
#include "mm.h"
#include "fs.h"
#include <string.h>

// System call table
typedef long (*syscall_func_t)(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t, uint64_t);

static syscall_func_t syscall_table[256];

// Set up system call table entry
#define SYSCALL_ENTRY(num, func) syscall_table[num] = (syscall_func_t)func

// System call implementations (kernel side)
static long sys_read(uint64_t fd, uint64_t buf, uint64_t count, uint64_t unused1, uint64_t unused2, uint64_t unused3) {
    return syscall_read(fd, (void*)buf, count);
}

static long sys_write(uint64_t fd, uint64_t buf, uint64_t count, uint64_t unused1, uint64_t unused2, uint64_t unused3) {
    return syscall_write(fd, (void*)buf, count);
}

static long sys_open(uint64_t pathname, uint64_t flags, uint64_t mode, uint64_t unused1, uint64_t unused2, uint64_t unused3) {
    return syscall_open((const char*)pathname, flags, mode);
}

static long sys_close(uint64_t fd, uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5) {
    return syscall_close(fd);
}

static long sys_lseek(uint64_t fd, uint64_t offset, uint64_t whence, uint64_t unused1, uint64_t unused2, uint64_t unused3) {
    return syscall_lseek(fd, offset, whence);
}

static long sys_stat(uint64_t pathname, uint64_t statbuf, uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4) {
    return syscall_stat((const char*)pathname, (struct stat*)statbuf);
}

static long sys_fork(uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6) {
    return syscall_fork();
}

static long sys_execve(uint64_t path, uint64_t argv, uint64_t envp, uint64_t unused1, uint64_t unused2, uint64_t unused3) {
    return syscall_execve((const char*)path, (char*const*)argv, (char*const*)envp);
}

static long sys_waitpid(uint64_t pid, uint64_t status, uint64_t options, uint64_t unused1, uint64_t unused2, uint64_t unused3) {
    return syscall_waitpid(pid, (int*)status, options);
}

static long sys_exit(uint64_t status, uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5) {
    syscall_exit(status);
    return 0;  // Never reached
}

static long sys_mmap(uint64_t addr, uint64_t length, uint64_t prot, uint64_t flags, uint64_t fd, uint64_t offset) {
    return (long)syscall_mmap((void*)addr, length, prot, flags, fd, offset);
}

static long sys_munmap(uint64_t addr, uint64_t length, uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4) {
    return syscall_munmap((void*)addr, length);
}

static long sys_brk(uint64_t addr, uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5) {
    return (long)syscall_brk((void*)addr);
}

static long sys_signal(uint64_t signum, uint64_t handler, uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4) {
    syscall_signal(signum, (void(*)(int))handler);
    return 0;
}

static long sys_sigaction(uint64_t signum, uint64_t act, uint64_t oldact, uint64_t unused1, uint64_t unused2, uint64_t unused3) {
    return syscall_sigaction(signum, (const struct sigaction*)act, (struct sigaction*)oldact);
}

static long sys_kill(uint64_t pid, uint64_t signum, uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4) {
    return syscall_kill(pid, signum);
}

static long sys_pthread_create(uint64_t thread, uint64_t attr, uint64_t start_routine, uint64_t arg, uint64_t unused1, uint64_t unused2) {
    return syscall_pthread_create((uint32_t*)thread, (void*(*)(void*))start_routine, (void*)arg);
}

static long sys_pthread_join(uint64_t thread, uint64_t retval, uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4) {
    return syscall_pthread_join(thread, (void**)retval);
}

static long sys_pthread_mutex_init(uint64_t mutex, uint64_t attr, uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4) {
    return syscall_pthread_mutex_init((uint32_t*)mutex);
}

static long sys_pthread_mutex_lock(uint64_t mutex, uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5) {
    return syscall_pthread_mutex_lock(mutex);
}

static long sys_pthread_mutex_unlock(uint64_t mutex, uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5) {
    return syscall_pthread_mutex_unlock(mutex);
}

static long sys_socket(uint64_t domain, uint64_t type, uint64_t protocol, uint64_t unused1, uint64_t unused2, uint64_t unused3) {
    return syscall_socket(domain, type, protocol);
}

static long sys_bind(uint64_t sockfd, uint64_t addr, uint64_t addrlen, uint64_t unused1, uint64_t unused2, uint64_t unused3) {
    return syscall_bind(sockfd, (const struct sockaddr*)addr, addrlen);
}

static long sys_listen(uint64_t sockfd, uint64_t backlog, uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4) {
    return syscall_listen(sockfd, backlog);
}

static long sys_accept(uint64_t sockfd, uint64_t addr, uint64_t addrlen, uint64_t unused1, uint64_t unused2, uint64_t unused3) {
    return syscall_accept(sockfd, (struct sockaddr*)addr, (socklen_t*)addrlen);
}

static long sys_connect(uint64_t sockfd, uint64_t addr, uint64_t addrlen, uint64_t unused1, uint64_t unused2, uint64_t unused3) {
    return syscall_connect(sockfd, (const struct sockaddr*)addr, addrlen);
}

static long sys_send(uint64_t sockfd, uint64_t buf, uint64_t len, uint64_t flags, uint64_t unused1, uint64_t unused2) {
    return syscall_send(sockfd, (void*)buf, len, flags);
}

static long sys_recv(uint64_t sockfd, uint64_t buf, uint64_t len, uint64_t flags, uint64_t unused1, uint64_t unused2) {
    return syscall_recv(sockfd, (void*)buf, len, flags);
}

static long sys_fcntl(uint64_t fd, uint64_t cmd, uint64_t arg, uint64_t unused1, uint64_t unused2, uint64_t unused3) {
    return syscall_fcntl(fd, cmd, arg);
}

static long sys_ioctl(uint64_t fd, uint64_t request, uint64_t arg, uint64_t unused1, uint64_t unused2, uint64_t unused3) {
    return syscall_ioctl(fd, request, arg);
}

static long sys_select(uint64_t nfds, uint64_t readfds, uint64_t writefds, uint64_t exceptfds, uint64_t timeout, uint64_t unused1) {
    return syscall_select(nfds, (fd_set*)readfds, (fd_set*)writefds, (fd_set*)exceptfds, (struct timeval*)timeout);
}

static long sys_poll(uint64_t fds, uint64_t nfds, uint64_t timeout, uint64_t unused1, uint64_t unused2, uint64_t unused3) {
    return syscall_poll((struct pollfd*)fds, nfds, timeout);
}

static long sys_mprotect(uint64_t addr, uint64_t len, uint64_t prot, uint64_t unused1, uint64_t unused2, uint64_t unused3) {
    return syscall_mprotect((void*)addr, len, prot);
}

static long sys_getuid(uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6) {
    return syscall_getuid();
}

static long sys_getgid(uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5, uint64_t unused6) {
    return syscall_getgid();
}

static long sys_setuid(uint64_t uid, uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5) {
    return syscall_setuid(uid);
}

static long sys_setgid(uint64_t gid, uint64_t unused1, uint64_t unused2, uint64_t unused3, uint64_t unused4, uint64_t unused5) {
    return syscall_setgid(gid);
}

// Initialize syscall system
void syscall_init(void) {
    // Clear syscall table
    for (int i = 0; i < 256; i++) {
        syscall_table[i] = NULL;
    }
    
    // Register system calls
    SYSCALL_ENTRY(SYS_READ, sys_read);
    SYSCALL_ENTRY(SYS_WRITE, sys_write);
    SYSCALL_ENTRY(SYS_OPEN, sys_open);
    SYSCALL_ENTRY(SYS_CLOSE, sys_close);
    SYSCALL_ENTRY(SYS_LSEEK, sys_lseek);
    SYSCALL_ENTRY(SYS_STAT, sys_stat);
    SYSCALL_ENTRY(SYS_FORK, sys_fork);
    SYSCALL_ENTRY(SYS_EXECVE, sys_execve);
    SYSCALL_ENTRY(SYS_WAITPID, sys_waitpid);
    SYSCALL_ENTRY(SYS_EXIT, sys_exit);
    SYSCALL_ENTRY(SYS_MMAP, sys_mmap);
    SYSCALL_ENTRY(SYS_MUNMAP, sys_munmap);
    SYSCALL_ENTRY(SYS_BRK, sys_brk);
    SYSCALL_ENTRY(SYS_SIGNAL, sys_signal);
    SYSCALL_ENTRY(SYS_SIGACTION, sys_sigaction);
    SYSCALL_ENTRY(SYS_KILL, sys_kill);
    SYSCALL_ENTRY(SYS_PTHREAD_CREATE, sys_pthread_create);
    SYSCALL_ENTRY(SYS_PTHREAD_JOIN, sys_pthread_join);
    SYSCALL_ENTRY(SYS_PTHREAD_MUTEX_INIT, sys_pthread_mutex_init);
    SYSCALL_ENTRY(SYS_PTHREAD_MUTEX_LOCK, sys_pthread_mutex_lock);
    SYSCALL_ENTRY(SYS_PTHREAD_MUTEX_UNLOCK, sys_pthread_mutex_unlock);
    SYSCALL_ENTRY(SYS_SOCKET, sys_socket);
    SYSCALL_ENTRY(SYS_BIND, sys_bind);
    SYSCALL_ENTRY(SYS_LISTEN, sys_listen);
    SYSCALL_ENTRY(SYS_ACCEPT, sys_accept);
    SYSCALL_ENTRY(SYS_CONNECT, sys_connect);
    SYSCALL_ENTRY(SYS_SEND, sys_send);
    SYSCALL_ENTRY(SYS_RECV, sys_recv);
    SYSCALL_ENTRY(SYS_FCNTL, sys_fcntl);
    SYSCALL_ENTRY(SYS_IOCTL, sys_ioctl);
    SYSCALL_ENTRY(SYS_SELECT, sys_select);
    SYSCALL_ENTRY(SYS_POLL, sys_poll);
    SYSCALL_ENTRY(SYS_MPROTECT, sys_mprotect);
    SYSCALL_ENTRY(SYS_GETUID, sys_getuid);
    SYSCALL_ENTRY(SYS_GETGID, sys_getgid);
    SYSCALL_ENTRY(SYS_SETUID, sys_setuid);
    SYSCALL_ENTRY(SYS_SETGID, sys_setgid);
}

// System call handler
void syscall_handler(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3, 
                     uint64_t arg4, uint64_t arg5, uint64_t arg6) {
    if (syscall_num >= 256 || syscall_table[syscall_num] == NULL) {
        // Invalid system call
        return;
    }
    
    // Call system call function
    long result = syscall_table[syscall_num](arg1, arg2, arg3, arg4, arg5, arg6);
    
    // Set return value in RAX
    asm volatile("mov %0, %%rax" : : "r" (result));
}

// User-space syscall wrappers
ssize_t read(int fd, void *buf, size_t count) {
    return SYSCALL3(SYS_READ, fd, buf, count);
}

ssize_t write(int fd, const void *buf, size_t count) {
    return SYSCALL3(SYS_WRITE, fd, buf, count);
}

int open(const char *pathname, int flags, mode_t mode) {
    return SYSCALL3(SYS_OPEN, pathname, flags, mode);
}

int close(int fd) {
    return SYSCALL1(SYS_CLOSE, fd);
}

off_t lseek(int fd, off_t offset, int whence) {
    return SYSCALL3(SYS_LSEEK, fd, offset, whence);
}

int stat(const char *pathname, struct stat *statbuf) {
    return SYSCALL2(SYS_STAT, pathname, statbuf);
}

pid_t fork(void) {
    return SYSCALL0(SYS_FORK);
}

int execve(const char *path, char *const argv[], char *const envp[]) {
    return SYSCALL3(SYS_EXECVE, path, argv, envp);
}

pid_t waitpid(pid_t pid, int *status, int options) {
    return SYSCALL3(SYS_WAITPID, pid, status, options);
}

void exit(int status) {
    SYSCALL1(SYS_EXIT, status);
}

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
    return (void*)SYSCALL6(SYS_MMAP, addr, length, prot, flags, fd, offset);
}

int munmap(void *addr, size_t length) {
    return SYSCALL2(SYS_MUNMAP, addr, length);
}

void *brk(void *addr) {
    return (void*)SYSCALL1(SYS_BRK, addr);
}

void *sbrk(ptrdiff_t increment) {
    void *old_brk = (void*)SYSCALL1(SYS_BRK, 0);
    void *new_brk = (void*)((uint64_t)old_brk + increment);
    SYSCALL1(SYS_BRK, new_brk);
    return old_brk;
}

void (*signal(int signum, void (*handler)(int)))(int) {
    return (void(*)(int))SYSCALL2(SYS_SIGNAL, signum, handler);
}

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
    return SYSCALL3(SYS_SIGACTION, signum, act, oldact);
}

int kill(pid_t pid, int signum) {
    return SYSCALL2(SYS_KILL, pid, signum);
}

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, 
                  void *(*start_routine)(void *), void *arg) {
    return SYSCALL4(SYS_PTHREAD_CREATE, thread, attr, start_routine, arg);
}

int pthread_join(pthread_t thread, void **retval) {
    return SYSCALL2(SYS_PTHREAD_JOIN, thread, retval);
}

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr) {
    return SYSCALL2(SYS_PTHREAD_MUTEX_INIT, mutex, attr);
}

int pthread_mutex_lock(pthread_mutex_t *mutex) {
    return SYSCALL1(SYS_PTHREAD_MUTEX_LOCK, mutex);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex) {
    return SYSCALL1(SYS_PTHREAD_MUTEX_UNLOCK, mutex);
}

int socket(int domain, int type, int protocol) {
    return SYSCALL3(SYS_SOCKET, domain, type, protocol);
}

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    return SYSCALL3(SYS_BIND, sockfd, addr, addrlen);
}

int listen(int sockfd, int backlog) {
    return SYSCALL2(SYS_LISTEN, sockfd, backlog);
}

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
    return SYSCALL3(SYS_ACCEPT, sockfd, addr, addrlen);
}

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    return SYSCALL3(SYS_CONNECT, sockfd, addr, addrlen);
}

ssize_t send(int sockfd, const void *buf, size_t len, int flags) {
    return SYSCALL4(SYS_SEND, sockfd, buf, len, flags);
}

ssize_t recv(int sockfd, void *buf, size_t len, int flags) {
    return SYSCALL4(SYS_RECV, sockfd, buf, len, flags);
}

int fcntl(int fd, int cmd, ...) {
    va_list args;
    va_start(args, cmd);
    long arg = va_arg(args, long);
    va_end(args);
    return SYSCALL3(SYS_FCNTL, fd, cmd, arg);
}

int ioctl(int fd, unsigned long request, ...) {
    va_list args;
    va_start(args, request);
    long arg = va_arg(args, long);
    va_end(args);
    return SYSCALL3(SYS_IOCTL, fd, request, arg);
}

int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
    return SYSCALL5(SYS_SELECT, nfds, readfds, writefds, exceptfds, timeout);
}

int poll(struct pollfd *fds, nfds_t nfds, int timeout) {
    return SYSCALL3(SYS_POLL, fds, nfds, timeout);
}

int mprotect(void *addr, size_t len, int prot) {
    return SYSCALL3(SYS_MPROTECT, addr, len, prot);
}

uid_t getuid(void) {
    return SYSCALL0(SYS_GETUID);
}

gid_t getgid(void) {
    return SYSCALL0(SYS_GETGID);
}

int setuid(uid_t uid) {
    return SYSCALL1(SYS_SETUID, uid);
}

int setgid(gid_t gid) {
    return SYSCALL1(SYS_SETGID, gid);
}
