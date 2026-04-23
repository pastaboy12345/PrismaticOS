#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "mm.h"

// Process states
#define PROC_STATE_RUNNING    0
#define PROC_STATE_SLEEPING   1
#define PROC_STATE_TERMINATED 2
#define PROC_STATE_ZOMBIE     3

// Thread states
#define THREAD_STATE_RUNNING   0
#define THREAD_STATE_READY     1
#define THREAD_STATE_BLOCKED   2
#define THREAD_STATE_TERMINATED 3

// Signal definitions
#define SIGINT    2
#define SIGKILL   9
#define SIGTERM  15
#define SIGCHLD  17
#define SIGSTOP  19
#define SIGCONT  20


// Thread structure
typedef struct thread {
    uint32_t tid;
    uint32_t pid;
    uint8_t state;
    
    // Stack
    void *stack_base;
    void *stack_top;
    uint64_t rsp;
    
    // Execution context
    uint64_t rip;
    uint64_t rbx, rbp, r12, r13, r14, r15;
    
    // Scheduling
    uint8_t priority;
    uint64_t time_slice;
    uint64_t last_run;
    
    // Synchronization
    struct thread *waiting_on;
    void *wait_object;
    
    struct thread *next;
    struct thread *prev;
} thread_t;

// Mutex structure
typedef struct mutex {
    uint32_t owner;
    uint32_t lock_count;
    thread_t *wait_queue;
    bool initialized;
} mutex_t;

// Process management functions
void process_init(void);
process_t *process_create(void);
void process_destroy(process_t *proc);
process_t *process_get_current(void);
void process_send_signal(process_t *proc, int signum);
void process_handle_signals(process_t *proc);

// Thread management functions
thread_t *thread_create(process_t *proc, void *(*start_routine)(void *), void *arg);
void thread_destroy(thread_t *thread);
thread_t *thread_get_current(void);
void thread_yield(void);
void thread_sleep(uint64_t milliseconds);
void thread_block(thread_t *thread);
void thread_unblock(thread_t *thread);
void thread_set_priority(thread_t *thread, uint8_t priority);

// Scheduling functions
void scheduler_init(void);
void scheduler_start(void);
void scheduler_add_thread(thread_t *thread);
void scheduler_remove_thread(thread_t *thread);
void scheduler_schedule(void);
void scheduler_tick(void);

// Context switching
void context_switch(thread_t *from, thread_t *to);
void thread_entry_wrapper(void *(*start_routine)(void *), void *arg);

// Mutex functions
int mutex_init(mutex_t *mutex);
int mutex_lock(mutex_t *mutex);
int mutex_unlock(mutex_t *mutex);
int mutex_trylock(mutex_t *mutex);
void mutex_destroy(mutex_t *mutex);

// Process list management
extern process_t *process_list;
extern thread_t *ready_queue;
extern thread_t *current_thread;

#endif // PROCESS_H
