#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <stdarg.h>

// Libc comprehensive test
static void test_memory_allocation(void) {
    printf("Testing memory allocation...\n");
    
    // Test various allocation sizes
    size_t sizes[] = {1, 8, 64, 512, 4096, 32768};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    for (int i = 0; i < num_sizes; i++) {
        void *ptr = malloc(sizes[i]);
        if (ptr) {
            // Write pattern
            memset(ptr, 0x5A, sizes[i]);
            
            // Verify pattern
            char *bytes = (char*)ptr;
            int valid = 1;
            for (size_t j = 0; j < sizes[i]; j++) {
                if (bytes[j] != 0x5A) {
                    valid = 0;
                    break;
                }
            }
            
            printf("  malloc(%zu): %s\n", sizes[i], valid ? "PASS" : "FAIL");
            free(ptr);
        } else {
            printf("  malloc(%zu): FAIL\n", sizes[i]);
        }
    }
    
    // Test calloc
    void *ptr = calloc(100, sizeof(int));
    if (ptr) {
        int *ints = (int*)ptr;
        int all_zero = 1;
        for (int i = 0; i < 100; i++) {
            if (ints[i] != 0) {
                all_zero = 0;
                break;
            }
        }
        printf("  calloc(100, sizeof(int)): %s\n", all_zero ? "PASS" : "FAIL");
        free(ptr);
    }
    
    // Test realloc
    ptr = malloc(100);
    if (ptr) {
        ptr = realloc(ptr, 200);
        if (ptr) {
            printf("  realloc expansion: PASS\n");
            free(ptr);
        } else {
            printf("  realloc expansion: FAIL\n");
        }
    }
    
    printf("Memory allocation test completed\n\n");
}

static void test_string_operations(void) {
    printf("Testing string operations...\n");
    
    // Test strlen
    const char *test_str = "Hello, World!";
    size_t len = strlen(test_str);
    printf("  strlen(\"%s\"): %zu %s\n", test_str, len, 
           (len == 13) ? "PASS" : "FAIL");
    
    // Test strcpy/strcat
    char buffer[256];
    strcpy(buffer, "Hello");
    strcat(buffer, ", ");
    strcat(buffer, "World");
    strcat(buffer, "!");
    printf("  strcpy/strcat: \"%s\" %s\n", buffer,
           (strcmp(buffer, test_str) == 0) ? "PASS" : "FAIL");
    
    // Test strcmp
    int cmp = strcmp("abc", "abc");
    printf("  strcmp(\"abc\", \"abc\"): %d %s\n", cmp, (cmp == 0) ? "PASS" : "FAIL");
    
    cmp = strcmp("abc", "def");
    printf("  strcmp(\"abc\", \"def\"): %d %s\n", cmp, (cmp < 0) ? "PASS" : "FAIL");
    
    // Test memcpy
    char src[] = "source";
    char dst[10];
    memcpy(dst, src, sizeof(src));
    printf("  memcpy: \"%s\" %s\n", dst, (strcmp(src, dst) == 0) ? "PASS" : "FAIL");
    
    // Test memset
    memset(dst, 'X', 5);
    dst[5] = '\0';
    printf("  memset: \"%s\" %s\n", dst, (strncmp(dst, "XXXXX", 5) == 0) ? "PASS" : "FAIL");
    
    printf("String operations test completed\n\n");
}

static void test_stdio_operations(void) {
    printf("Testing stdio operations...\n");
    
    // Test printf formatting
    int result = printf("  printf test: %d, %s, %.2f\n", 42, "hello", 3.14159);
    printf("  printf return value: %d\n", result);
    
    // Test snprintf
    char buffer[100];
    result = snprintf(buffer, sizeof(buffer), "snprintf: %d + %d = %d", 2, 3, 5);
    printf("  snprintf result: \"%s\" (len=%d)\n", buffer, result);
    
    // Test file operations
    FILE *fp = fopen("/tmp/test_file", "w");
    if (fp) {
        fprintf(fp, "Test content\n");
        fclose(fp);
        
        fp = fopen("/tmp/test_file", "r");
        if (fp) {
            char line[100];
            if (fgets(line, sizeof(line), fp)) {
                printf("  file read: \"%s\" %s\n", line,
                       (strcmp(line, "Test content\n") == 0) ? "PASS" : "FAIL");
            }
            fclose(fp);
        }
        
        unlink("/tmp/test_file");
    } else {
        printf("  file operations: SKIP (could not create test file)\n");
    }
    
    printf("Stdio operations test completed\n\n");
}

static void test_character_functions(void) {
    printf("Testing character functions...\n");
    
    char test_chars[] = "Aa1! \t\n";
    int expected_alnum[] = {1, 1, 1, 0, 0, 0, 0};
    int expected_alpha[] = {1, 1, 0, 0, 0, 0, 0};
    int expected_digit[] = {0, 0, 1, 0, 0, 0, 0};
    int expected_space[] = {0, 0, 0, 0, 1, 1, 1};
    
    int all_pass = 1;
    for (int i = 0; i < 7; i++) {
        char c = test_chars[i];
        int alnum = isalnum(c);
        int alpha = isalpha(c);
        int digit = isdigit(c);
        int space = isspace(c);
        
        printf("  '%c': alnum=%d(%d), alpha=%d(%d), digit=%d(%d), space=%d(%d)",
               c, alnum, expected_alnum[i], alpha, expected_alpha[i],
               digit, expected_digit[i], space, expected_space[i]);
        
        if (alnum == expected_alnum[i] && alpha == expected_alpha[i] &&
            digit == expected_digit[i] && space == expected_space[i]) {
            printf(" PASS\n");
        } else {
            printf(" FAIL\n");
            all_pass = 0;
        }
    }
    
    printf("Character functions test: %s\n\n", all_pass ? "PASS" : "FAIL");
}

static void test_number_conversion(void) {
    printf("Testing number conversion...\n");
    
    // Test atoi
    int result = atoi("12345");
    printf("  atoi(\"12345\"): %d %s\n", result, (result == 12345) ? "PASS" : "FAIL");
    
    result = atoi("-123");
    printf("  atoi(\"-123\"): %d %s\n", result, (result == -123) ? "PASS" : "FAIL");
    
    // Test strtol
    char *endptr;
    long val = strtol("12345", &endptr, 10);
    printf("  strtol(\"12345\"): %ld %s\n", val, (val == 12345) ? "PASS" : "FAIL");
    
    // Test base conversion
    val = strtol("FF", &endptr, 16);
    printf("  strtol(\"FF\", 16): %ld %s\n", val, (val == 255) ? "PASS" : "FAIL");
    
    // Test atof
    double dval = atof("3.14159");
    printf("  atof(\"3.14159\"): %.5f %s\n", dval, 
           (dval > 3.14158 && dval < 3.14160) ? "PASS" : "FAIL");
    
    printf("Number conversion test completed\n\n");
}

static void test_errno_handling(void) {
    printf("Testing errno handling...\n");
    
    // Clear errno
    errno = 0;
    
    // Force an error
    close(-1);
    
    printf("  close(-1) errno: %d %s\n", errno, (errno != 0) ? "PASS" : "FAIL");
    
    // Test strerror
    const char *msg = strerror(errno);
    printf("  strerror(%d): \"%s\" %s\n", errno, msg, (msg != NULL) ? "PASS" : "FAIL");
    
    printf("Errno handling test completed\n\n");
}

static void test_random_functions(void) {
    printf("Testing random functions...\n");
    
    // Seed the random number generator
    srand(42);
    
    // Test rand
    int r1 = rand();
    int r2 = rand();
    int r3 = rand();
    
    printf("  rand() sequence: %d, %d, %d\n", r1, r2, r3);
    
    // Test range
    int in_range = 0;
    for (int i = 0; i < 1000; i++) {
        int r = rand();
        if (r >= 0 && r <= RAND_MAX) {
            in_range++;
        }
    }
    
    printf("  rand() range test: %d/1000 in range %s\n", in_range,
           (in_range == 1000) ? "PASS" : "FAIL");
    
    printf("Random functions test completed\n\n");
}

static void test_exit_functions(void) {
    printf("Testing exit functions...\n");
    
    // Note: We can't actually test exit() since it would terminate the program
    printf("  atexit registration: ");
    if (atexit((void(*)())(void*)0) == 0) {
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }
    
    printf("Exit functions test completed\n\n");
}

int main(void) {
    printf("=== Libc Test Suite ===\n\n");
    
    test_memory_allocation();
    test_string_operations();
    test_stdio_operations();
    test_character_functions();
    test_number_conversion();
    test_errno_handling();
    test_random_functions();
    test_exit_functions();
    
    printf("=== All libc tests completed! ===\n");
    return 0;
}
