#ifndef SYS_TYPES_H
#define SYS_TYPES_H

#include <stddef.h>

// Basic types
typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;
typedef long long int int64_t;
typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

// Process types
typedef int pid_t;
typedef unsigned int uid_t;
typedef unsigned int gid_t;
typedef long int off_t;
typedef long long int off64_t;
typedef unsigned long int size_t;
typedef long int ssize_t;
typedef long int time_t;
typedef long int suseconds_t;
typedef int id_t;
typedef int mode_t;

// File system types
typedef unsigned long int ino_t;
typedef unsigned long int dev_t;
typedef unsigned long int nlink_t;
typedef unsigned long int blksize_t;
typedef unsigned long int blkcnt_t;

// Process ID types
typedef enum {
    P_ALL,
    P_PID,
    P_PGID
} idtype_t;

// Signal types
typedef struct {
    int si_signo;
    int si_errno;
    int si_code;
    union {
        pid_t si_pid;
        void *si_timerid;
        void *si_addr;
    } _sifields;
} siginfo_t;

// System information
struct utsname {
    char sysname[65];
    char nodename[65];
    char release[65];
    char version[65];
    char machine[65];
    char domainname[65];
};

// File status
struct stat {
    dev_t st_dev;
    ino_t st_ino;
    mode_t st_mode;
    nlink_t st_nlink;
    uid_t st_uid;
    gid_t st_gid;
    dev_t st_rdev;
    off_t st_size;
    blksize_t st_blksize;
    blkcnt_t st_blocks;
    time_t st_atime;
    time_t st_mtime;
    time_t st_ctime;
};

// Time structures
struct timespec {
    time_t tv_sec;
    long tv_nsec;
};

struct timeval {
    time_t tv_sec;
    suseconds_t tv_usec;
};

// Resource limits
typedef unsigned long rlim_t;
typedef long useconds_t;

struct rlimit {
    rlim_t rlim_cur;
    rlim_t rlim_max;
};

#endif // SYS_TYPES_H
