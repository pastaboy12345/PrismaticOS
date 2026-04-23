#ifndef SIGNAL_H
#define SIGNAL_H

#include <sys/types.h>

// Signal types
#define SIGINT     2   // Interrupt
#define SIGQUIT    3   // Quit
#define SIGILL     4   // Illegal instruction
#define SIGTRAP    5   // Trace trap
#define SIGABRT    6   // Abort
#define SIGBUS     7   // BUS error
#define SIGFPE     8   // Floating-point exception
#define SIGKILL    9   // Kill, unblockable
#define SIGUSR1   10   // User-defined signal 1
#define SIGSEGV   11   // Segmentation violation
#define SIGUSR2   12   // User-defined signal 2
#define SIGPIPE   13   // Broken pipe
#define SIGALRM   14   // Alarm clock
#define SIGTERM   15   // Termination
#define SIGSTKFLT 16   // Stack fault
#define SIGCHLD   17   // Child status has changed
#define SIGCONT   18   // Continue
#define SIGSTOP   19   // Stop, unblockable
#define SIGTSTP   20   // Keyboard stop
#define SIGTTIN   21   // Background read from tty
#define SIGTTOU   22   // Background write to tty

// Signal action structure
struct sigaction {
    void (*sa_handler)(int);
    void (*sa_sigaction)(int, siginfo_t *, void *);
    sigset_t sa_mask;
    int sa_flags;
    void (*sa_restorer)(void);
};

// Signal set type
typedef unsigned long sigset_t;

// Signal handling functions
void (*signal(int signum, void (*handler)(int)))(int);
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
int sigpending(sigset_t *set);
int sigsuspend(const sigset_t *mask);

// Signal manipulation functions
int kill(pid_t pid, int signum);
int raise(int signum);
int sigqueue(pid_t pid, int signum, const union sigval value);

// Signal set operations
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set, int signum);
int sigdelset(sigset_t *set, int signum);
int sigismember(const sigset_t *set, int signum);

// Signal constants
#define SIG_DFL ((void (*)(int))0)   // Default signal handler
#define SIG_IGN ((void (*)(int))1)   // Ignore signal
#define SIG_ERR ((void (*)(int))-1)  // Error return from signal

// Signal flags
#define SA_NOCLDSTOP    0x00000001  // Don't send SIGCHLD when children stop
#define SA_NOCLDWAIT    0x00000002  // Don't create zombie children
#define SA_SIGINFO      0x00000004  // Invoke signal-catching function with three arguments
#define SA_RESTART      0x10000000  // Restart syscall if possible
#define SA_RESETHAND    0x80000000  // Reset to SIG_DFL when signal is caught

// Signal mask operations
#define SIG_BLOCK       0  // Block signals
#define SIG_UNBLOCK     1  // Unblock signals
#define SIG_SETMASK     2  // Set the signal mask

#endif // SIGNAL_H
