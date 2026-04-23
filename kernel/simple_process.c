// Simple 32-bit process management
#include "kernel.h"
#include <string.h>

// Forward declarations
void *kmalloc(size_t size);
void kfree(void *ptr);

// Simple process structure
typedef struct simple_process {
    uint32_t pid;
    uint8_t state;
    struct simple_process *next;
} simple_process_t;

static simple_process_t *current_process = NULL;
static uint32_t next_pid = 1;

void process_init(void) {
    // Create kernel process
    current_process = kmalloc(sizeof(simple_process_t));
    if (current_process) {
        current_process->pid = next_pid++;
        current_process->state = 0; // running
        current_process->next = NULL;
    }
}

process_t *process_create(void) {
    simple_process_t *proc = kmalloc(sizeof(simple_process_t));
    if (!proc) return NULL;
    
    proc->pid = next_pid++;
    proc->state = 0; // running
    proc->next = NULL;
    
    return (process_t*)proc;
}

void process_destroy(process_t *proc) {
    kfree(proc);
}

process_t *process_get_current(void) {
    return (process_t*)current_process;
}

void process_switch(process_t *next) {
    current_process = (simple_process_t*)next;
}

void scheduler_start(void) {
    // Simple scheduler - just keep running current process
    while (1) {
        __asm__ volatile("hlt");
    }
}
