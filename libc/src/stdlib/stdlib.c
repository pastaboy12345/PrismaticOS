#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Exit functions
void exit(int status) {
    _exit(status);
}

void _Exit(int status) {
    syscall(SYS_exit, status);
}

void abort(void) {
    // Raise SIGABRT
    raise(SIGABRT);
    
    // If signal handler returns, terminate
    _exit(128 + SIGABRT);
}

// Random number generation (simple linear congruential generator)
static unsigned long next = 1;

int rand(void) {
    next = next * 1103515245 + 12345;
    return (unsigned int)(next / 65536) % 32768;
}

void srand(unsigned int seed) {
    next = seed;
}

int rand_r(unsigned int *seed) {
    unsigned long next_val = *seed;
    next_val = next_val * 1103515245 + 12345;
    *seed = (unsigned int)next_val;
    return (unsigned int)(next_val / 65536) % 32768;
}

// Integer arithmetic
int abs(int x) {
    return x < 0 ? -x : x;
}

long labs(long x) {
    return x < 0 ? -x : x;
}

long long llabs(long long x) {
    return x < 0 ? -x : x;
}

div_t div(int numer, int denom) {
    div_t result;
    result.quot = numer / denom;
    result.rem = numer % denom;
    return result;
}

ldiv_t ldiv(long numer, long denom) {
    ldiv_t result;
    result.quot = numer / denom;
    result.rem = numer % denom;
    return result;
}

lldiv_t lldiv(long long numer, long long denom) {
    lldiv_t result;
    result.quot = numer / denom;
    result.rem = numer % denom;
    return result;
}

// Environment (simplified)
char *getenv(const char *name) {
    // For now, return NULL for all environment variables
    // In a real implementation, this would access the environment
    return NULL;
}

int putenv(char *string) {
    // For now, return success
    // In a real implementation, this would modify the environment
    return 0;
}

int setenv(const char *name, const char *value, int overwrite) {
    // For now, return success
    // In a real implementation, this would modify the environment
    return 0;
}

int unsetenv(const char *name) {
    // For now, return success
    // In a real implementation, this would modify the environment
    return 0;
}

int clearenv(void) {
    // For now, return success
    // In a real implementation, this would clear the environment
    return 0;
}

// System command (simplified)
int system(const char *command) {
    if (!command) {
        return 1;  // Shell is available
    }
    
    // For simplicity, just return success
    // In a real implementation, this would fork and exec the shell
    return 0;
}

// Path resolution (simplified)
char *realpath(const char *pathname, char *resolved_path) {
    // For simplicity, just copy the path
    if (!resolved_path) {
        resolved_path = malloc(PATH_MAX);
        if (!resolved_path) return NULL;
    }
    
    strcpy(resolved_path, pathname);
    return resolved_path;
}

// Atexit handlers (simplified)
#define MAX_ATEXIT_HANDLERS 32

static struct {
    void (*handlers[MAX_ATEXIT_HANDLERS])(void);
    int count;
} atexit_data = { {NULL}, 0 };

int atexit(void (*function)(void)) {
    if (atexit_data.count >= MAX_ATEXIT_HANDLERS) {
        return -1;
    }
    
    atexit_data.handlers[atexit_data.count++] = function;
    return 0;
}

int at_quick_exit(void (*function)(void)) {
    // For simplicity, just use atexit
    return atexit(function);
}

void quick_exit(int status) {
    // Call at_quick_exit handlers
    for (int i = atexit_data.count - 1; i >= 0; i--) {
        if (atexit_data.handlers[i]) {
            atexit_data.handlers[i]();
        }
    }
    
    _exit(status);
}

// Execute atexit handlers (called by exit)
static void run_atexit_handlers(void) {
    for (int i = atexit_data.count - 1; i >= 0; i--) {
        if (atexit_data.handlers[i]) {
            atexit_data.handlers[i]();
        }
    }
}
