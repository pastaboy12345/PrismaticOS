#ifndef FS_H
#define FS_H

#include <stdint.h>
#include <stddef.h>

// Type definitions
typedef int32_t ssize_t;
typedef int32_t mode_t;
typedef int64_t off_t;

// Forward declaration
typedef struct fs_ops fs_ops_t;

// File types
#define S_IFMT        00170000    // File type mask
#define S_IFREG       0100000     // Regular file
#define S_IFDIR       0040000     // Directory
#define S_IFCHR       0020000     // Character device
#define S_IFBLK       0060000     // Block device
#define S_IFIFO       0010000     // FIFO
#define S_IFLNK       0120000     // Symbolic link
#define S_IFSOCK      0140000     // Socket

// File permissions
#define S_ISUID       0004000     // Set user ID
#define S_ISGID       0002000     // Set group ID
#define S_ISVTX       0001000     // Sticky bit
#define S_IRWXU       0000700     // Owner: read, write, execute
#define S_IRUSR       0000400     // Owner: read
#define S_IWUSR       0000200     // Owner: write
#define S_IXUSR       0000100     // Owner: execute
#define S_IRWXG       0000070     // Group: read, write, execute
#define S_IRGRP       0000040     // Group: read
#define S_IWGRP       0000020     // Group: write
#define S_IXGRP       0000010     // Group: execute
#define S_IRWXO       0000007     // Others: read, write, execute
#define S_IROTH       0000004     // Others: read
#define S_IWOTH       0000002     // Others: write
#define S_IXOTH       0000001     // Others: execute

// Open flags
#define O_RDONLY      00000000
#define O_WRONLY      00000001
#define O_RDWR        00000002
#define O_CREAT       00000100
#define O_EXCL        00000200
#define O_NOCTTY      00000400
#define O_TRUNC       00001000
#define O_APPEND      00002000
#define O_NONBLOCK    00004000
#define O_SYNC        00010000
#define O_ASYNC       00020000

// Seek origins
#define SEEK_SET      0
#define SEEK_CUR      1
#define SEEK_END      2


// File descriptor structure
typedef struct file_descriptor {
    uint32_t fd;
    void *file_object;
    uint32_t flags;
    uint32_t mode;
    uint64_t offset;
    struct file_descriptor *next;
} file_descriptor_t;

// File operations structure
typedef struct file_ops {
    ssize_t (*read)(void *file_object, void *buf, size_t count, uint64_t offset);
    ssize_t (*write)(void *file_object, const void *buf, size_t count, uint64_t offset);
    int (*ioctl)(void *file_object, unsigned long request, void *arg);
    int (*close)(void *file_object);
} file_ops_t;

// File object structure
typedef struct file_object {
    uint32_t inode;
    uint32_t mode;
    uint64_t size;
    uint64_t offset;
    file_ops_t *ops;
    void *private_data;
} file_object_t;

// Directory entry structure
typedef struct dir_entry {
    uint32_t inode;
    char name[256];
    uint8_t name_len;
    uint8_t file_type;
} dir_entry_t;

// Virtual file system node
typedef struct vfs_node {
    char name[256];
    uint32_t inode;
    uint32_t mode;
    uint64_t size;
    uint64_t atime;
    uint64_t mtime;
    uint64_t ctime;
    struct vfs_node *parent;
    struct vfs_node *children;
    struct vfs_node *next;
    file_ops_t *ops;
    void *private_data;
} vfs_node_t;

// File system structure
typedef struct filesystem {
    char name[32];
    char mountpoint[256];
    vfs_node_t *root;
    fs_ops_t *fs_ops;
    struct filesystem *next;
} filesystem_t;

// File system operations
typedef struct fs_ops {
    vfs_node_t *(*mount)(const char *device, const char *mountpoint);
    int (*unmount)(const char *mountpoint);
    vfs_node_t *(*lookup)(vfs_node_t *parent, const char *name);
    int (*create)(vfs_node_t *parent, const char *name, uint32_t mode);
    int (*unlink)(vfs_node_t *parent, const char *name);
    int (*mkdir)(vfs_node_t *parent, const char *name, uint32_t mode);
    int (*rmdir)(vfs_node_t *parent, const char *name);
} fs_ops_t;

// File system functions
void fs_init(void);
int fs_register_filesystem(const char *name, fs_ops_t *ops);
int fs_mount(const char *fs_name, const char *device, const char *mountpoint);
int fs_unmount(const char *mountpoint);
vfs_node_t *fs_lookup(const char *path);
vfs_node_t *fs_create(const char *path, uint32_t mode);
int fs_unlink(const char *path);
int fs_mkdir(const char *path, uint32_t mode);
int fs_rmdir(const char *path);

// File descriptor management
int fd_alloc(void);
void fd_free(int fd);
file_descriptor_t *fd_get(int fd);
int fd_set_entry(int fd, file_object_t *file_object, uint32_t flags, uint32_t mode);

// Virtual file system functions
vfs_node_t *vfs_create_node(const char *name, uint32_t mode, file_ops_t *ops);
void vfs_destroy_node(vfs_node_t *node);
vfs_node_t *vfs_add_child(vfs_node_t *parent, vfs_node_t *child);
vfs_node_t *vfs_find_child(vfs_node_t *parent, const char *name);
int vfs_remove_child(vfs_node_t *parent, const char *name);

// File operations
ssize_t file_read(vfs_node_t *node, void *buf, size_t count, uint64_t offset);
ssize_t file_write(vfs_node_t *node, const void *buf, size_t count, uint64_t offset);
int file_ioctl(vfs_node_t *node, unsigned long request, void *arg);
int file_close(vfs_node_t *node);

// Directory operations
int dir_read(vfs_node_t *node, dir_entry_t *entry, uint64_t offset);
int dir_write(vfs_node_t *node, const dir_entry_t *entry, uint64_t offset);

// Path utilities
int path_normalize(const char *path, char *normalized);
int path_is_absolute(const char *path);
char *path_basename(const char *path);
char *path_dirname(const char *path);

// System call implementations
int syscall_open(const char *pathname, int flags, mode_t mode);
int syscall_close(int fd);
ssize_t syscall_read(int fd, void *buf, size_t count);
ssize_t syscall_write(int fd, const void *buf, size_t count);
off_t syscall_lseek(int fd, off_t offset, int whence);

#endif // FS_H
