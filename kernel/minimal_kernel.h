#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Kernel functions
void kernel_main(void);
void panic(const char *message);

#endif // KERNEL_H
