#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

// Standard file streams
FILE *stdin = NULL;
FILE *stdout = NULL;
FILE *stderr = NULL;

// Initialize standard streams
static void init_stdio(void) {
    static int initialized = 0;
    if (initialized) return;
    
    // Create stdin
    stdin = malloc(sizeof(FILE));
    if (stdin) {
        stdin->fd = STDIN_FILENO;
        stdin->flags = FILE_TYPE_READ;
        stdin->buffer = NULL;
        stdin->buffer_size = 0;
        stdin->buffer_pos = 0;
        stdin->buffer_used = 0;
        stdin->eof = 0;
        stdin->error = 0;
    }
    
    // Create stdout
    stdout = malloc(sizeof(FILE));
    if (stdout) {
        stdout->fd = STDOUT_FILENO;
        stdout->flags = FILE_TYPE_WRITE;
        stdout->buffer = NULL;
        stdout->buffer_size = 0;
        stdout->buffer_pos = 0;
        stdout->buffer_used = 0;
        stdout->eof = 0;
        stdout->error = 0;
    }
    
    // Create stderr
    stderr = malloc(sizeof(FILE));
    if (stderr) {
        stderr->fd = STDERR_FILENO;
        stderr->flags = FILE_TYPE_WRITE;
        stderr->buffer = NULL;
        stderr->buffer_size = 0;
        stderr->buffer_pos = 0;
        stderr->buffer_used = 0;
        stderr->eof = 0;
        stderr->error = 0;
    }
    
    initialized = 1;
}

// Parse file mode
static int parse_mode(const char *mode) {
    int flags = 0;
    
    if (!mode) return -1;
    
    switch (mode[0]) {
        case 'r':
            flags = FILE_TYPE_READ;
            if (mode[1] == '+') {
                flags |= FILE_TYPE_WRITE;
            }
            break;
        case 'w':
            flags = FILE_TYPE_WRITE;
            if (mode[1] == '+') {
                flags |= FILE_TYPE_READ;
            }
            break;
        case 'a':
            flags = FILE_TYPE_APPEND;
            if (mode[1] == '+') {
                flags |= FILE_TYPE_READ;
            }
            break;
        default:
            return -1;
    }
    
    return flags;
}

// Convert mode flags to open flags
static int mode_to_open_flags(int mode_flags) {
    int open_flags = 0;
    
    if (mode_flags & FILE_TYPE_READ) {
        if (mode_flags & FILE_TYPE_WRITE) {
            open_flags = O_RDWR;
        } else {
            open_flags = O_RDONLY;
        }
    } else if (mode_flags & FILE_TYPE_WRITE) {
        open_flags = O_WRONLY | O_CREAT | O_TRUNC;
    } else if (mode_flags & FILE_TYPE_APPEND) {
        open_flags = O_WRONLY | O_CREAT | O_APPEND;
    }
    
    return open_flags;
}

// fopen implementation
FILE *fopen(const char *pathname, const char *mode) {
    init_stdio();
    
    int mode_flags = parse_mode(mode);
    if (mode_flags < 0) {
        return NULL;
    }
    
    int open_flags = mode_to_open_flags(mode_flags);
    int fd = open(pathname, open_flags, 0644);
    if (fd < 0) {
        return NULL;
    }
    
    return fdopen(fd, mode);
}

// fdopen implementation
FILE *fdopen(int fd, const char *mode) {
    init_stdio();
    
    int mode_flags = parse_mode(mode);
    if (mode_flags < 0) {
        return NULL;
    }
    
    FILE *stream = malloc(sizeof(FILE));
    if (!stream) {
        return NULL;
    }
    
    stream->fd = fd;
    stream->flags = mode_flags;
    stream->buffer = NULL;
    stream->buffer_size = 0;
    stream->buffer_pos = 0;
    stream->buffer_used = 0;
    stream->eof = 0;
    stream->error = 0;
    
    return stream;
}

// fclose implementation
int fclose(FILE *stream) {
    if (!stream) return EOF;
    
    // Flush any buffered data
    if (stream->flags & (FILE_TYPE_WRITE | FILE_TYPE_APPEND)) {
        if (stream->buffer && stream->buffer_used > 0) {
            write(stream->fd, stream->buffer, stream->buffer_used);
        }
    }
    
    // Close file descriptor
    int result = close(stream->fd);
    
    // Free buffer
    if (stream->buffer) {
        free(stream->buffer);
    }
    
    // Free FILE structure
    free(stream);
    
    return result == 0 ? 0 : EOF;
}

// fread implementation
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    if (!ptr || !stream || size == 0 || nmemb == 0) {
        return 0;
    }
    
    size_t total_bytes = size * nmemb;
    size_t bytes_read = 0;
    
    // Check if stream is opened for reading
    if (!(stream->flags & FILE_TYPE_READ)) {
        stream->error = 1;
        return 0;
    }
    
    // Read directly without buffering for simplicity
    ssize_t result = read(stream->fd, ptr, total_bytes);
    if (result < 0) {
        stream->error = 1;
        return 0;
    } else if (result == 0) {
        stream->eof = 1;
        return 0;
    }
    
    bytes_read = result;
    return bytes_read / size;
}

// fwrite implementation
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
    if (!ptr || !stream || size == 0 || nmemb == 0) {
        return 0;
    }
    
    size_t total_bytes = size * nmemb;
    size_t bytes_written = 0;
    
    // Check if stream is opened for writing
    if (!(stream->flags & (FILE_TYPE_WRITE | FILE_TYPE_APPEND))) {
        stream->error = 1;
        return 0;
    }
    
    // Write directly without buffering for simplicity
    ssize_t result = write(stream->fd, ptr, total_bytes);
    if (result < 0) {
        stream->error = 1;
        return 0;
    }
    
    bytes_written = result;
    return bytes_written / size;
}

// fseek implementation
int fseek(FILE *stream, long offset, int whence) {
    if (!stream) {
        return -1;
    }
    
    off_t result = lseek(stream->fd, offset, whence);
    if (result < 0) {
        stream->error = 1;
        return -1;
    }
    
    // Clear EOF flag
    stream->eof = 0;
    
    return 0;
}

// ftell implementation
long ftell(FILE *stream) {
    if (!stream) {
        return -1;
    }
    
    off_t result = lseek(stream->fd, 0, SEEK_CUR);
    if (result < 0) {
        stream->error = 1;
        return -1;
    }
    
    return result;
}

// rewind implementation
void rewind(FILE *stream) {
    if (stream) {
        fseek(stream, 0, SEEK_SET);
        stream->error = 0;
        stream->eof = 0;
    }
}

// fgetc implementation
int fgetc(FILE *stream) {
    if (!stream) {
        return EOF;
    }
    
    unsigned char c;
    size_t result = fread(&c, 1, 1, stream);
    if (result != 1) {
        return EOF;
    }
    
    return c;
}

// fputc implementation
int fputc(int c, FILE *stream) {
    if (!stream) {
        return EOF;
    }
    
    unsigned char ch = c;
    size_t result = fwrite(&ch, 1, 1, stream);
    if (result != 1) {
        return EOF;
    }
    
    return c;
}

// fgets implementation
char *fgets(char *s, int size, FILE *stream) {
    if (!s || !stream || size <= 0) {
        return NULL;
    }
    
    char *ptr = s;
    int c;
    
    while (size > 1 && (c = fgetc(stream)) != EOF) {
        *ptr++ = c;
        size--;
        
        if (c == '\n') {
            break;
        }
    }
    
    if (ptr == s) {
        return NULL;  // No characters read
    }
    
    *ptr = '\0';
    return s;
}

// fputs implementation
int fputs(const char *s, FILE *stream) {
    if (!s || !stream) {
        return EOF;
    }
    
    size_t len = strlen(s);
    size_t result = fwrite(s, 1, len, stream);
    if (result != len) {
        return EOF;
    }
    
    return 0;
}

// Error handling functions
void clearerr(FILE *stream) {
    if (stream) {
        stream->error = 0;
        stream->eof = 0;
    }
}

int feof(FILE *stream) {
    return stream ? stream->eof : 1;
}

int ferror(FILE *stream) {
    return stream ? stream->error : 1;
}

void perror(const char *s) {
    if (s) {
        fprintf(stderr, "%s: ", s);
    }
    fprintf(stderr, "%s\n", strerror(errno));
}
