.section .text

// System call wrapper macros
#define SYSCALL(name, num) \
    .global syscall_##name; \
    syscall_##name: \
        mov %rax, num; \
        syscall; \
        ret

// File operations
SYSCALL(read, 0)
SYSCALL(write, 1)
SYSCALL(open, 2)
SYSCALL(close, 3)
SYSCALL(stat, 4)
SYSCALL(lseek, 6)
SYSCALL(mmap, 7)
SYSCALL(munmap, 8)
SYSCALL(brk, 9)
SYSCALL(fork, 54)
SYSCALL(execve, 56)
SYSCALL(waitpid, 58)
SYSCALL(exit, 57)
SYSCALL(kill, 59)
SYSCALL(socket, 38)
SYSCALL(bind, 46)
SYSCALL(listen, 47)
SYSCALL(accept, 40)
SYSCALL(connect, 39)
SYSCALL(send, 41)
SYSCALL(recv, 42)
SYSCALL(fcntl, 68)
SYSCALL(ioctl, 13)
SYSCALL(getuid, 94)
SYSCALL(getgid, 100)
SYSCALL(setuid, 101)
SYSCALL(setgid, 102)
SYSCALL(mprotect, 125)
