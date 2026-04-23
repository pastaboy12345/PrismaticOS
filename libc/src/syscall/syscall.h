#ifndef SYSCALL_H
#define SYSCALL_H

#include <sys/types.h>

// System call wrapper macros
#define syscall(num, ...) syscall_##num(__VA_ARGS__)

// System call numbers
#define SYS_read        0
#define SYS_write       1
#define SYS_open        2
#define SYS_close       3
#define SYS_stat        4
#define SYS_fstat       5
#define SYS_lseek       6
#define SYS_mmap        7
#define SYS_munmap      8
#define SYS_brk         9
#define SYS_rt_sigaction 10
#define SYS_rt_sigprocmask 11
#define SYS_rt_sigreturn 12
#define SYS_ioctl       13
#define SYS_pread64     14
#define SYS_pwrite64    15
#define SYS_readv       16
#define SYS_writev      17
#define SYS_access      18
#define SYS_pipe        19
#define SYS_select      20
#define SYS_sched_yield 21
#define SYS_mremap      22
#define SYS_msync       23
#define SYS_mincore     24
#define SYS_madvise     25
#define SYS_shmget      26
#define SYS_shmat       27
#define SYS_shmctl      28
#define SYS_dup         29
#define SYS_dup2        30
#define SYS_pause       31
#define SYS_nanosleep   32
#define SYS_getitimer   33
#define SYS_alarm       34
#define SYS_setitimer   35
#define SYS_getpid      36
#define SYS_sendfile    37
#define SYS_socket      38
#define SYS_connect     39
#define SYS_accept      40
#define SYS_sendto      41
#define SYS_recvfrom    42
#define SYS_sendmsg     43
#define SYS_recvmsg     44
#define SYS_shutdown    45
#define SYS_bind        46
#define SYS_listen      47
#define SYS_getsockname 48
#define SYS_getpeername 49
#define SYS_socketpair  50
#define SYS_setsockopt  51
#define SYS_getsockopt  52
#define SYS_clone       53
#define SYS_fork        54
#define SYS_vfork       55
#define SYS_execve      56
#define SYS_exit        57
#define SYS_wait4       58
#define SYS_kill        59
#define SYS_uname       60
#define SYS_semget      61
#define SYS_semop       62
#define SYS_semctl      63
#define SYS_shmdt       64
#define SYS_msgget      65
#define SYS_msgsnd      66
#define SYS_msgrcv      67
#define SYS_fcntl       68
#define SYS_flock       69
#define SYS_fsync       70
#define SYS_fdatasync   71
#define SYS_truncate    72
#define SYS_ftruncate   73
#define SYS_getdents    74
#define SYS_getcwd      75
#define SYS_chdir       76
#define SYS_fchdir      77
#define SYS_rename      78
#define SYS_mkdir       79
#define SYS_rmdir       80
#define SYS_creat       81
#define SYS_link        82
#define SYS_unlink      83
#define SYS_symlink     84
#define SYS_readlink    85
#define SYS_chmod       86
#define SYS_fchmod      87
#define SYS_chown       88
#define SYS_fchown      89
#define SYS_lchown      90
#define SYS_umask       91
#define SYS_gettimeofday 92
#define SYS_getrlimit   93
#define SYS_getuid      94
#define SYS_sysinfo     95
#define SYS_times       96
#define SYS_ptrace      97
#define SYS_getuid      98
#define SYS_syslog      99
#define SYS_getgid      100
#define SYS_setuid      101
#define SYS_setgid      102
#define SYS_seteuid     103
#define SYS_setegid     104
#define SYS_getppid     105
#define SYS_getpgrp     106
#define SYS_setsid      107
#define SYS_setreuid    108
#define SYS_setregid    109
#define SYS_getgroups   110
#define SYS_setgroups   111
#define SYS_setresuid   112
#define SYS_getresuid   113
#define SYS_setresgid   114
#define SYS_getresgid   115
#define SYS_getpgid     116
#define SYS_setfsuid    117
#define SYS_setfsgid    118
#define SYS_getsid      119
#define SYS_setresuid   120
#define SYS_getresuid   121
#define SYS_setresgid   122
#define SYS_getresgid   123
#define SYS_getgroups   124
#define SYS_setgroups   125
#define SYS_fchown      126
#define SYS_setresuid   127
#define SYS_getresuid   128
#define SYS_setresgid   129
#define SYS_getresgid   130
#define SYS_lchown      131
#define SYS_setuid      132
#define SYS_setgid      133
#define SYS_setfsuid    134
#define SYS_setfsgid    135

// System call assembly wrappers
long syscall_read(int fd, void *buf, size_t count);
long syscall_write(int fd, const void *buf, size_t count);
long syscall_open(const char *pathname, int flags, mode_t mode);
long syscall_close(int fd);
long syscall_stat(const char *pathname, struct stat *statbuf);
long syscall_lseek(int fd, off_t offset, int whence);
long syscall_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
long syscall_munmap(void *addr, size_t length);
long syscall_brk(void *addr);
long syscall_fork(void);
long syscall_execve(const char *path, char *const argv[], char *const envp[]);
long syscall_waitpid(pid_t pid, int *status, int options);
long syscall_exit(int status);
long syscall_kill(pid_t pid, int signum);
long syscall_socket(int domain, int type, int protocol);
long syscall_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
long syscall_listen(int sockfd, int backlog);
long syscall_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
long syscall_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
long syscall_send(int sockfd, const void *buf, size_t len, int flags);
long syscall_recv(int sockfd, void *buf, size_t len, int flags);
long syscall_fcntl(int fd, int cmd, ...);
long syscall_ioctl(int fd, unsigned long request, ...);
long syscall_getuid(void);
long syscall_getgid(void);
long syscall_setuid(uid_t uid);
long syscall_setgid(gid_t gid);

#endif // SYSCALL_H
