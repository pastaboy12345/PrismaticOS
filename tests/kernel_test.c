#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

// Simple kernel test
static void test_syscalls(void) {
    printf("Testing system calls...\n");
    
    // Test write
    const char *msg = "Hello from kernel test!\n";
    ssize_t written = write(STDOUT_FILENO, msg, strlen(msg));
    printf("write() returned %zd bytes\n", written);
    
    // Test getpid
    pid_t pid = getpid();
    printf("getpid() returned %d\n", pid);
    
    // Test getuid
    uid_t uid = getuid();
    printf("getuid() returned %d\n", uid);
    
    printf("System calls test completed\n\n");
}

static void test_memory(void) {
    printf("Testing memory allocation...\n");
    
    // Test malloc
    void *ptr = malloc(1024);
    if (ptr) {
        printf("malloc(1024) succeeded\n");
        memset(ptr, 0xAA, 1024);
        free(ptr);
        printf("free() completed\n");
    } else {
        printf("malloc(1024) failed\n");
    }
    
    // Test calloc
    ptr = calloc(100, sizeof(int));
    if (ptr) {
        printf("calloc(100, sizeof(int)) succeeded\n");
        free(ptr);
    } else {
        printf("calloc() failed\n");
    }
    
    // Test realloc
    ptr = malloc(100);
    if (ptr) {
        ptr = realloc(ptr, 200);
        if (ptr) {
            printf("realloc() succeeded\n");
            free(ptr);
        } else {
            printf("realloc() failed\n");
        }
    }
    
    printf("Memory allocation test completed\n\n");
}

static void test_string_functions(void) {
    printf("Testing string functions...\n");
    
    const char *test_str = "Hello, World!";
    size_t len = strlen(test_str);
    printf("strlen(\"%s\") = %zu\n", test_str, len);
    
    char *copy = strdup(test_str);
    if (copy) {
        printf("strdup() created: %s\n", copy);
        free(copy);
    }
    
    char buffer[256];
    strcpy(buffer, test_str);
    printf("strcpy() result: %s\n", buffer);
    
    strcat(buffer, " - Test");
    printf("strcat() result: %s\n", buffer);
    
    printf("String functions test completed\n\n");
}

static void test_stdio(void) {
    printf("Testing stdio functions...\n");
    
    printf("printf() test: %d + %d = %d\n", 5, 3, 5 + 3);
    
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "snprintf() test: %s", "success");
    printf("%s\n", buffer);
    
    FILE *fp = fopen("/dev/null", "w");
    if (fp) {
        fprintf(fp, "Test output\n");
        fclose(fp);
        printf("File operations test completed\n");
    } else {
        printf("File operations test failed\n");
    }
    
    printf("Stdio test completed\n\n");
}

static void test_ctype(void) {
    printf("Testing character functions...\n");
    
    char test_chars[] = "A1! ";
    for (int i = 0; i < 4; i++) {
        char c = test_chars[i];
        printf("'%c': isalnum=%d, isalpha=%d, isdigit=%d, isspace=%d\n",
               c, isalnum(c), isalpha(c), isdigit(c), isspace(c));
    }
    
    printf("Character functions test completed\n\n");
}

static void test_errno(void) {
    printf("Testing errno handling...\n");
    
    // Force an error
    close(-1);
    printf("After close(-1), errno = %d\n", errno);
    
    printf("Errno test completed\n\n");
}

int main(void) {
    printf("=== Kernel Test Suite ===\n\n");
    
    test_syscalls();
    test_memory();
    test_string_functions();
    test_stdio();
    test_ctype();
    test_errno();
    
    printf("=== All kernel tests completed successfully! ===\n");
    return 0;
}
