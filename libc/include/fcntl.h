#ifndef FCNTL_H
#define FCNTL_H

#include <sys/types.h>

// File control flags
#define O_RDONLY        0x00000000  // Read-only
#define O_WRONLY        0x00000001  // Write-only
#define O_RDWR          0x00000002  // Read-write
#define O_ACCMODE       0x00000003  // Mask for file access modes

#define O_CREAT         0x00000040  // Create if doesn't exist
#define O_EXCL          0x00000080  // Exclusive use
#define O_NOCTTY        0x00000100  // Don't assign controlling terminal
#define O_TRUNC         0x00000200  // Truncate
#define O_APPEND        0x00000400  // Append
#define O_NONBLOCK      0x00000800  // Non-blocking

#define O_DSYNC         0x00001000  // Synchronize data
#define O_ASYNC         0x00002000  // Asynchronous I/O

#define O_DIRECTORY     0x00010000  // Must be a directory
#define O_NOFOLLOW      0x00020000  // Don't follow links
#define O_CLOEXEC       0x00040000  // Close on exec

// File control commands
#define F_DUPFD         0       // Duplicate file descriptor
#define F_GETFD         1       // Get file descriptor flags
#define F_SETFD         2       // Set file descriptor flags
#define F_GETFL         3       // Get file status flags
#define F_SETFL         4       // Set file status flags
#define F_GETLK         5       // Get record locking info
#define F_SETLK         6       // Set record locking info
#define F_SETLKW        7       // Set record locking info (wait)

// File descriptor flags
#define FD_CLOEXEC      1       // Close on exec

// Lock types
#define F_RDLCK         0       // Read lock
#define F_WRLCK         1       // Write lock
#define F_UNLCK         2       // Unlock

// Advisory file locking
struct flock {
    short l_type;       // Type of lock: F_RDLCK, F_WRLCK, F_UNLCK
    short l_whence;     // Where to start lock: SEEK_SET, SEEK_CUR, SEEK_END
    off_t l_start;      // Starting offset
    off_t l_len;        // Number of bytes, 0 = to EOF
    pid_t l_pid;        // PID of process blocking the lock
};

// File control functions
int fcntl(int fd, int cmd, ...);
int open(const char *pathname, int flags, ...);
int creat(const char *pathname, mode_t mode);

#endif // FCNTL_H
