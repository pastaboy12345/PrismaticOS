#ifndef UNISTD_H
#define UNISTD_H

#include <stddef.h>
#include <sys/types.h>

// Standard file descriptors
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

// File operations
int close(int fd);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
int fsync(int fd);
int fdatasync(int fd);
off_t lseek(int fd, off_t offset, int whence);
int ftruncate(int fd, off_t length);

// File descriptor manipulation
int dup(int oldfd);
int dup2(int oldfd, int newfd);
int dup3(int oldfd, int newfd, int flags);

// Process operations
pid_t fork(void);
int execve(const char *pathname, char *const argv[], char *const envp[]);
int execv(const char *pathname, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execl(const char *pathname, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execle(const char *pathname, const char *arg, ...);
void exit(int status);
void _exit(int status);
pid_t getpid(void);
pid_t getppid(void);
pid_t getpgrp(void);
pid_t getpgid(pid_t pid);
int setpgid(pid_t pid, pid_t pgid);
pid_t setsid(void);
pid_t getsid(pid_t pid);
uid_t getuid(void);
uid_t geteuid(void);
gid_t getgid(void);
gid_t getegid(void);
int setuid(uid_t uid);
int seteuid(uid_t uid);
int setgid(gid_t gid);
int setegid(gid_t gid);

// Process waiting
pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);
int waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);

// Process groups and sessions
pid_t tcgetpgrp(int fd);
int tcsetpgrp(int fd, pid_t pgrp);
char *getcwd(char *buf, size_t size);
char *get_current_dir_name(void);
int chdir(const char *path);
int fchdir(int fd);
int chroot(const char *path);

// User and group information
int getgroups(int size, gid_t list[]);
int setgroups(size_t size, const gid_t *list);
int setresuid(uid_t ruid, uid_t euid, uid_t suid);
int getresuid(uid_t *ruid, uid_t *euid, uid_t *suid);
int setresgid(gid_t rgid, gid_t egid, gid_t sgid);
int getresgid(gid_t *rgid, gid_t *egid, gid_t *sgid);

// Environment variables
int setenv(const char *name, const char *value, int overwrite);
int unsetenv(const char *name);
int clearenv(void);

// System information
int uname(struct utsname *buf);
int gethostname(char *name, size_t len);
int getdomainname(char *name, size_t len);
int setdomainname(const char *name, size_t len);
int getloadavg(double loadavg[], int nelem);

// Memory operations
void *sbrk(ptrdiff_t increment);
int brk(void *addr);

// Symbolic links
ssize_t readlink(const char *pathname, char *buf, size_t bufsiz);
int symlink(const char *target, const char *linkpath);
int unlink(const char *pathname);
int rmdir(const char *pathname);
int rename(const char *oldpath, const char *newpath);

// Directory operations
int mkdir(const char *pathname, mode_t mode);
int rmdir(const char *pathname);
int chown(const char *pathname, uid_t owner, gid_t group);
int fchown(int fd, uid_t owner, gid_t group);
int lchown(const char *pathname, uid_t owner, gid_t group);
int chmod(const char *pathname, mode_t mode);
int fchmod(int fd, mode_t mode);
int fchmodat(int dirfd, const char *pathname, mode_t mode, int flags);

// File status
int stat(const char *pathname, struct stat *statbuf);
int fstat(int fd, struct stat *statbuf);
int lstat(const char *pathname, struct stat *statbuf);
int fstatat(int dirfd, const char *pathname, struct stat *statbuf, int flags);

// Access control
int access(const char *pathname, int mode);
int faccessat(int dirfd, const char *pathname, int mode, int flags);

// Pipe and FIFO
int pipe(int pipefd[2]);
int pipe2(int pipefd[2], int flags);

// Memory mapping
void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);
void *mremap(void *old_address, size_t old_size, size_t new_size, int flags, ...);
int msync(void *addr, size_t length, int flags);
int mprotect(void *addr, size_t len, int prot);

// Truncate
int truncate(const char *path, off_t length);
int ftruncate(int fd, off_t length);

// Sync
void sync(void);

// Sleep
unsigned int sleep(unsigned int seconds);
int usleep(useconds_t usec);

// Alarm
unsigned int alarm(unsigned int seconds);

// Pause
int pause(void);

#endif // UNISTD_H
