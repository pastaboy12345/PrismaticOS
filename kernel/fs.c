#include "fs.h"
#include "kernel.h"
#include "process.h"
#include <string.h>

// Global file system state
static filesystem_t *filesystems = NULL;
static vfs_node_t *vfs_root = NULL;
static file_descriptor_t *fd_table[256] = {NULL};

// Simple in-memory file system for testing
static vfs_node_t *memfs_root = NULL;

// File operations for in-memory files
static ssize_t memfs_read(void *file_object, void *buf, size_t count, uint64_t offset);
static ssize_t memfs_write(void *file_object, const void *buf, size_t count, uint64_t offset);
static int memfs_ioctl(void *file_object, unsigned long request, void *arg);
static int memfs_close(void *file_object);

static file_ops_t memfs_file_ops = {
    .read = memfs_read,
    .write = memfs_write,
    .ioctl = memfs_ioctl,
    .close = memfs_close
};

// Initialize file system
void fs_init(void) {
    // Clear file descriptor table
    for (int i = 0; i < 256; i++) {
        fd_table[i] = NULL;
    }
    
    // Create root VFS node
    vfs_root = vfs_create_node("/", S_IFDIR | 0755, &memfs_file_ops);
    if (!vfs_root) {
        panic("Failed to create VFS root");
    }
    
    // Create in-memory file system
    memfs_root = vfs_create_node("memfs", S_IFDIR | 0755, &memfs_file_ops);
    if (!memfs_root) {
        panic("Failed to create memfs root");
    }
    
    // Mount memfs at root
    vfs_add_child(vfs_root, memfs_root);
    
    // Create some basic files
    vfs_node_t *hello_file = vfs_create_node("hello.txt", S_IFREG | 0644, &memfs_file_ops);
    if (hello_file) {
        // Allocate some content
        char *content = kmalloc(32);
        strcpy(content, "Hello, World!\nThis is a test file.\n");
        hello_file->private_data = content;
        hello_file->size = strlen(content);
        vfs_add_child(memfs_root, hello_file);
    }
    
    vfs_node_t *test_dir = vfs_create_node("test", S_IFDIR | 0755, &memfs_file_ops);
    if (test_dir) {
        vfs_add_child(memfs_root, test_dir);
        
        vfs_node_t *nested_file = vfs_create_node("nested.txt", S_IFREG | 0644, &memfs_file_ops);
        if (nested_file) {
            char *content = kmalloc(16);
            strcpy(content, "Nested file\n");
            nested_file->private_data = content;
            nested_file->size = strlen(content);
            vfs_add_child(test_dir, nested_file);
        }
    }
}

// Register a file system
int fs_register_filesystem(const char *name, fs_ops_t *ops) {
    filesystem_t *fs = kmalloc(sizeof(filesystem_t));
    if (!fs) return -1;
    
    strncpy(fs->name, name, sizeof(fs->name) - 1);
    fs->name[sizeof(fs->name) - 1] = '\0';
    fs->fs_ops = ops;
    fs->next = filesystems;
    filesystems = fs;
    
    return 0;
}

// Mount a file system
int fs_mount(const char *fs_name, const char *device, const char *mountpoint) {
    // Find file system
    filesystem_t *fs = filesystems;
    while (fs) {
        if (strcmp(fs->name, fs_name) == 0) {
            break;
        }
        fs = fs->next;
    }
    
    if (!fs) {
        return -1;  // File system not found
    }
    
    // Mount it
    vfs_node_t *mount_node = fs->fs_ops->mount(device, mountpoint);
    if (!mount_node) {
        return -1;
    }
    
    // Add to mount table
    strncpy(fs->mountpoint, mountpoint, sizeof(fs->mountpoint) - 1);
    fs->mountpoint[sizeof(fs->mountpoint) - 1] = '\0';
    fs->root = mount_node;
    
    return 0;
}

// Unmount a file system
int fs_unmount(const char *mountpoint) {
    filesystem_t *fs = filesystems;
    while (fs) {
        if (strcmp(fs->mountpoint, mountpoint) == 0) {
            if (fs->fs_ops->unmount) {
                fs->fs_ops->unmount(mountpoint);
            }
            fs->mountpoint[0] = '\0';
            fs->root = NULL;
            return 0;
        }
        fs = fs->next;
    }
    
    return -1;  // Not found
}

// Look up a path in the VFS
vfs_node_t *fs_lookup(const char *path) {
    if (!path || path[0] != '/') {
        return NULL;  // Must be absolute path
    }
    
    vfs_node_t *current = vfs_root;
    char path_copy[512];
    strncpy(path_copy, path, sizeof(path_copy) - 1);
    path_copy[sizeof(path_copy) - 1] = '\0';
    
    char *token = strtok(path_copy, "/");
    while (token && current) {
        if (strcmp(token, "") == 0) {
            token = strtok(NULL, "/");
            continue;
        }
        
        current = vfs_find_child(current, token);
        token = strtok(NULL, "/");
    }
    
    return current;
}

// Create a file or directory
vfs_node_t *fs_create(const char *path, uint32_t mode) {
    if (!path || path[0] != '/') {
        return NULL;
    }
    
    // Find parent directory
    char path_copy[512];
    char *dir_part = path_copy;
    strncpy(path_copy, path, sizeof(path_copy) - 1);
    path_copy[sizeof(path_copy) - 1] = '\0';
    
    // Find last '/' to separate directory from filename
    char *last_slash = strrchr(path_copy, '/');
    if (!last_slash || last_slash == path_copy) {
        return NULL;
    }
    
    *last_slash = '\0';
    char *filename = last_slash + 1;
    
    vfs_node_t *parent = fs_lookup(dir_part);
    if (!parent || !(parent->mode & S_IFDIR)) {
        return NULL;
    }
    
    // Create the node
    vfs_node_t *node = vfs_create_node(filename, mode, &memfs_file_ops);
    if (!node) {
        return NULL;
    }
    
    // Add to parent
    if (vfs_add_child(parent, node) == NULL) {
        vfs_destroy_node(node);
        return NULL;
    }
    
    return node;
}

// Unlink (remove) a file
int fs_unlink(const char *path) {
    vfs_node_t *node = fs_lookup(path);
    if (!node || (node->mode & S_IFDIR)) {
        return -1;  // Not found or is directory
    }
    
    // Remove from parent
    // TODO: implement removal
    return 0;
}

// Create a directory
int fs_mkdir(const char *path, uint32_t mode) {
    vfs_node_t *node = fs_create(path, S_IFDIR | (mode & 0777));
    return node ? 0 : -1;
}

// Remove a directory
int fs_rmdir(const char *path) {
    vfs_node_t *node = fs_lookup(path);
    if (!node || !(node->mode & S_IFDIR)) {
        return -1;  // Not found or not directory
    }
    
    // Check if directory is empty
    if (node->children) {
        return -1;  // Directory not empty
    }
    
    // Remove from parent
    // TODO: implement removal
    return 0;
}

// Allocate a file descriptor
int fd_alloc(void) {
    for (int i = 0; i < 256; i++) {
        if (!fd_table[i]) {
            file_descriptor_t *fd = kmalloc(sizeof(file_descriptor_t));
            if (!fd) return -1;
            
            fd->fd = i;
            fd->file_object = NULL;
            fd->flags = 0;
            fd->mode = 0;
            fd->offset = 0;
            fd->next = NULL;
            
            fd_table[i] = fd;
            return i;
        }
    }
    
    return -1;  // No free descriptors
}

// Free a file descriptor
void fd_free(int fd) {
    if (fd >= 0 && fd < 256 && fd_table[fd]) {
        kfree(fd_table[fd]);
        fd_table[fd] = NULL;
    }
}

// Get file descriptor
file_descriptor_t *fd_get(int fd) {
    if (fd >= 0 && fd < 256) {
        return fd_table[fd];
    }
    return NULL;
}

// Set file descriptor
int fd_set_entry(int fd, file_object_t *file_object, uint32_t flags, uint32_t mode) {
    file_descriptor_t *fd_entry = fd_get(fd);
    if (!fd_entry) return -1;
    
    fd_entry->file_object = file_object;
    fd_entry->flags = flags;
    fd_entry->mode = mode;
    fd_entry->offset = 0;
    
    return 0;
}

// Create a VFS node
vfs_node_t *vfs_create_node(const char *name, uint32_t mode, file_ops_t *ops) {
    vfs_node_t *node = kmalloc(sizeof(vfs_node_t));
    if (!node) return NULL;
    
    memset(node, 0, sizeof(vfs_node_t));
    strncpy(node->name, name, sizeof(node->name) - 1);
    node->name[sizeof(node->name) - 1] = '\0';
    node->inode = 0;  // TODO: generate unique inode
    node->mode = mode;
    node->size = 0;
    node->atime = node->mtime = node->ctime = 0;  // TODO: get current time
    node->parent = NULL;
    node->children = NULL;
    node->next = NULL;
    node->ops = ops;
    node->private_data = NULL;
    
    return node;
}

// Destroy a VFS node
void vfs_destroy_node(vfs_node_t *node) {
    if (!node) return;
    
    // Free private data
    if (node->private_data) {
        kfree(node->private_data);
    }
    
    // Free children recursively
    vfs_node_t *child = node->children;
    while (child) {
        vfs_node_t *next = child->next;
        vfs_destroy_node(child);
        child = next;
    }
    
    kfree(node);
}

// Add child to VFS node
vfs_node_t *vfs_add_child(vfs_node_t *parent, vfs_node_t *child) {
    if (!parent || !child) return NULL;
    
    child->parent = parent;
    child->next = parent->children;
    parent->children = child;
    
    return child;
}

// Find child by name
vfs_node_t *vfs_find_child(vfs_node_t *parent, const char *name) {
    if (!parent || !name) return NULL;
    
    vfs_node_t *child = parent->children;
    while (child) {
        if (strcmp(child->name, name) == 0) {
            return child;
        }
        child = child->next;
    }
    
    return NULL;
}

// Remove child by name
int vfs_remove_child(vfs_node_t *parent, const char *name) {
    if (!parent || !name) return -1;
    
    vfs_node_t *prev = NULL;
    vfs_node_t *child = parent->children;
    
    while (child) {
        if (strcmp(child->name, name) == 0) {
            if (prev) {
                prev->next = child->next;
            } else {
                parent->children = child->next;
            }
            
            vfs_destroy_node(child);
            return 0;
        }
        
        prev = child;
        child = child->next;
    }
    
    return -1;  // Not found
}

// In-memory file system operations
static ssize_t memfs_read(void *file_object, void *buf, size_t count, uint64_t offset) {
    vfs_node_t *node = (vfs_node_t*)file_object;
    if (!node || !node->private_data) return -1;
    
    char *data = (char*)node->private_data;
    size_t data_size = node->size;
    
    if (offset >= data_size) {
        return 0;  // EOF
    }
    
    size_t to_read = count;
    if (offset + to_read > data_size) {
        to_read = data_size - offset;
    }
    
    memcpy(buf, data + offset, to_read);
    return to_read;
}

static ssize_t memfs_write(void *file_object, const void *buf, size_t count, uint64_t offset) {
    vfs_node_t *node = (vfs_node_t*)file_object;
    if (!node) return -1;
    
    // Allocate or resize buffer
    char *data = (char*)node->private_data;
    size_t new_size = offset + count;
    
    if (new_size > node->size) {
        char *new_data = kmalloc(new_size);
        if (!new_data) return -1;
        
        if (data) {
            memcpy(new_data, data, node->size);
            kfree(data);
        }
        
        node->private_data = new_data;
        node->size = new_size;
        data = new_data;
    }
    
    memcpy(data + offset, buf, count);
    return count;
}

static int memfs_ioctl(void *file_object __attribute__((unused)), unsigned long request __attribute__((unused)), void *arg __attribute__((unused))) {
    return -1;  // Not implemented
}

static int memfs_close(void *file_object __attribute__((unused))) {
    // Nothing to close for memfs
    return 0;
}

// System call implementations
int syscall_open(const char *pathname, int flags, mode_t mode) {
    if (!pathname) return -1;
    
    // Look up the file
    vfs_node_t *node = fs_lookup(pathname);
    
    // Create file if O_CREAT is specified and file doesn't exist
    if (!node && (flags & O_CREAT)) {
        node = fs_create(pathname, S_IFREG | (mode & 0777));
    }
    
    if (!node) {
        return -1;  // File not found
    }
    
    // Check if it's a directory
    if (node->mode & S_IFDIR) {
        return -1;  // Can't open directories as files
    }
    
    // Truncate file if O_TRUNC is specified
    if ((flags & O_TRUNC) && node->private_data) {
        kfree(node->private_data);
        node->private_data = NULL;
        node->size = 0;
    }
    
    // Allocate file descriptor
    int fd = fd_alloc();
    if (fd < 0) {
        return -1;
    }
    
    // Set up file descriptor
    file_object_t *file_obj = kmalloc(sizeof(file_object_t));
    if (!file_obj) {
        fd_free(fd);
        return -1;
    }
    
    file_obj->inode = node->inode;
    file_obj->mode = node->mode;
    file_obj->size = node->size;
    file_obj->offset = 0;
    file_obj->ops = node->ops;
    file_obj->private_data = node;
    
    if (fd_set_entry(fd, file_obj, flags, mode) < 0) {
        kfree(file_obj);
        fd_free(fd);
        return -1;
    }
    
    return fd;
}

int syscall_close(int fd) {
    file_descriptor_t *fd_entry = fd_get(fd);
    if (!fd_entry) return -1;
    
    // Close file object
    if (fd_entry->file_object) {
        file_object_t *file_obj = (file_object_t*)fd_entry->file_object;
        if (file_obj->ops && file_obj->ops->close) {
            file_obj->ops->close(file_obj->private_data);
        }
        kfree(file_obj);
    }
    
    fd_free(fd);
    return 0;
}

ssize_t syscall_read(int fd, void *buf, size_t count) {
    file_descriptor_t *fd_entry = fd_get(fd);
    if (!fd_entry || !fd_entry->file_object) return -1;
    
    file_object_t *file_obj = (file_object_t*)fd_entry->file_object;
    if (!file_obj->ops || !file_obj->ops->read) return -1;
    
    ssize_t result = file_obj->ops->read(file_obj->private_data, buf, count, file_obj->offset);
    if (result > 0) {
        file_obj->offset += result;
        fd_entry->offset = file_obj->offset;
    }
    
    return result;
}

ssize_t syscall_write(int fd, const void *buf, size_t count) {
    file_descriptor_t *fd_entry = fd_get(fd);
    if (!fd_entry || !fd_entry->file_object) return -1;
    
    file_object_t *file_obj = (file_object_t*)fd_entry->file_object;
    if (!file_obj->ops || !file_obj->ops->write) return -1;
    
    ssize_t result = file_obj->ops->write(file_obj->private_data, buf, count, file_obj->offset);
    if (result > 0) {
        file_obj->offset += result;
        file_obj->size = file_obj->offset;
        fd_entry->offset = file_obj->offset;
    }
    
    return result;
}

off_t syscall_lseek(int fd, off_t offset, int whence) {
    file_descriptor_t *fd_entry = fd_get(fd);
    if (!fd_entry || !fd_entry->file_object) return -1;
    
    file_object_t *file_obj = (file_object_t*)fd_entry->file_object;
    
    uint64_t new_offset;
    switch (whence) {
        case SEEK_SET:
            new_offset = offset;
            break;
        case SEEK_CUR:
            new_offset = file_obj->offset + offset;
            break;
        case SEEK_END:
            new_offset = file_obj->size + offset;
            break;
        default:
            return -1;
    }
    
    if (new_offset > file_obj->size) {
        // Extend file if seeking beyond end
        if (file_obj->ops && file_obj->ops->write) {
            char zero = 0;
            while (file_obj->offset < new_offset) {
                if (file_obj->ops->write(file_obj->private_data, &zero, 1, file_obj->offset) != 1) {
                    return -1;
                }
                file_obj->offset++;
            }
        }
    }
    
    file_obj->offset = new_offset;
    fd_entry->offset = new_offset;
    
    return new_offset;
}

int syscall_stat(const char *pathname, struct stat *statbuf) {
    if (!pathname || !statbuf) return -1;
    
    vfs_node_t *node = fs_lookup(pathname);
    if (!node) return -1;
    
    memset(statbuf, 0, sizeof(struct stat));
    statbuf->st_mode = node->mode;
    statbuf->st_size = node->size;
    statbuf->st_atime = node->atime;
    statbuf->st_mtime = node->mtime;
    statbuf->st_ctime = node->ctime;
    
    return 0;
}
