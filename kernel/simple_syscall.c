// Simple 32-bit syscall handling
#include "kernel.h"

// Simple syscall stubs - all return -1 (not implemented)
int syscall_fork(void) { return -1; }
int syscall_execve(const char *path, char *const argv[], char *const envp[]) { (void)path; (void)argv; (void)envp; return -1; }
int syscall_waitpid(uint32_t pid, int *status, int options) { (void)pid; (void)status; (void)options; return -1; }
void syscall_exit(int status) { (void)status; }
void *syscall_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) { (void)addr; (void)length; (void)prot; (void)flags; (void)fd; (void)offset; return NULL; }
int syscall_munmap(void *addr, size_t length) { (void)addr; (void)length; return -1; }
void *syscall_brk(void *addr) { (void)addr; return NULL; }
int syscall_mprotect(void *addr, size_t len, int prot) { (void)addr; (void)len; (void)prot; return -1; }
int syscall_stat(const char *pathname, struct stat *statbuf) { (void)pathname; (void)statbuf; return -1; }
void syscall_signal(int signum, void (*handler)(int)) { (void)signum; (void)handler; }
int syscall_sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) { (void)signum; (void)act; (void)oldact; return -1; }
int syscall_kill(uint32_t pid, int signum) { (void)pid; (void)signum; return -1; }
int syscall_pthread_create(uint32_t *thread, void *(*start_routine)(void *), void *arg) { (void)thread; (void)start_routine; (void)arg; return -1; }
int syscall_pthread_join(uint32_t thread, void **retval) { (void)thread; (void)retval; return -1; }
int syscall_pthread_mutex_init(uint32_t *mutex) { (void)mutex; return -1; }
int syscall_pthread_mutex_lock(uint32_t mutex) { (void)mutex; return -1; }
int syscall_pthread_mutex_unlock(uint32_t mutex) { (void)mutex; return -1; }
int syscall_socket(int domain, int type, int protocol) { (void)domain; (void)type; (void)protocol; return -1; }
int syscall_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { (void)sockfd; (void)addr; (void)addrlen; return -1; }
int syscall_listen(int sockfd, int backlog) { (void)sockfd; (void)backlog; return -1; }
int syscall_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) { (void)sockfd; (void)addr; (void)addrlen; return -1; }
int syscall_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { (void)sockfd; (void)addr; (void)addrlen; return -1; }
ssize_t syscall_send(int sockfd, const void *buf, size_t len, int flags) { (void)sockfd; (void)buf; (void)len; (void)flags; return -1; }
ssize_t syscall_recv(int sockfd, void *buf, size_t len, int flags) { (void)sockfd; (void)buf; (void)len; (void)flags; return -1; }
int syscall_fcntl(int fd, int cmd, ...) { (void)fd; (void)cmd; return -1; }
int syscall_ioctl(int fd, unsigned long request, ...) { (void)fd; (void)request; return -1; }
int syscall_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) { (void)nfds; (void)readfds; (void)writefds; (void)exceptfds; (void)timeout; return -1; }
int syscall_poll(struct pollfd *fds, nfds_t nfds, int timeout) { (void)fds; (void)nfds; (void)timeout; return -1; }
