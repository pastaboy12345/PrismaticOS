#include "process.h"
#include "kernel.h"
#include "mm.h"
#include "syscall.h"
#include <string.h>

// Global process/thread lists
process_t *process_list = NULL;
thread_t *ready_queue = NULL;
thread_t *current_thread = NULL;

// Process and thread ID counters
static uint32_t next_pid = 1;
static uint32_t next_tid = 1;

// Current process (for fast access)
static process_t *current_process = NULL;

// Get current process
process_t *process_get_current(void) {
    return current_process;
}

// Get current thread
thread_t *thread_get_current(void) {
    return current_thread;
}

// Initialize process management
void process_init(void) {
    process_list = NULL;
    ready_queue = NULL;
    current_thread = NULL;
    current_process = NULL;
    next_pid = 1;
    next_tid = 1;
}

// Create a new process
process_t *process_create(void) {
    process_t *proc = kmalloc(sizeof(process_t));
    if (!proc) return NULL;
    
    // Initialize process structure
    memset(proc, 0, sizeof(process_t));
    proc->pid = next_pid++;
    proc->ppid = current_process ? current_process->pid : 0;
    proc->state = PROC_STATE_RUNNING;
    proc->start_time = 0;  // TODO: get system time
    proc->uid = 0;  // root
    proc->gid = 0;  // root
    
    // Create memory space
    proc->memory_space = mm_create_space();
    if (!proc->memory_space) {
        kfree(proc);
        return NULL;
    }
    
    // Initialize file descriptor table
    proc->fd_list = NULL;
    proc->fd_count = 0;
    
    // Initialize signal handlers
    for (int i = 0; i < 32; i++) {
        proc->signal_handlers[i] = NULL;
    }
    proc->pending_signals = 0;
    
    // Add to process list
    proc->next = process_list;
    if (process_list) {
        process_list->prev = proc;
    }
    process_list = proc;
    proc->prev = NULL;
    
    // Add to parent's children if parent exists
    if (current_process) {
        process_add_child(current_process, proc);
    }
    
    return proc;
}

// Destroy a process
void process_destroy(process_t *proc) {
    if (!proc) return;
    
    // Remove from process list
    if (proc->prev) {
        proc->prev->next = proc->next;
    } else {
        process_list = proc->next;
    }
    if (proc->next) {
        proc->next->prev = proc->prev;
    }
    
    // Remove from parent's children
    if (proc->parent) {
        process_remove_child(proc->parent, proc);
    }
    
    // Destroy all child processes
    process_t *child = proc->children;
    while (child) {
        process_t *next = child->next;
        process_destroy(child);
        child = next;
    }
    
    // Close all file descriptors
    // TODO: implement file descriptor cleanup
    
    // Destroy memory space
    if (proc->memory_space) {
        mm_destroy_space(proc->memory_space);
    }
    
    kfree(proc);
}

// Add child to parent
void process_add_child(process_t *parent, process_t *child) {
    child->parent = parent;
    child->next = parent->children;
    if (parent->children) {
        parent->children->prev = child;
    }
    parent->children = child;
    child->prev = NULL;
}

// Remove child from parent
void process_remove_child(process_t *parent, process_t *child) {
    if (child->prev) {
        child->prev->next = child->next;
    } else {
        parent->children = child->next;
    }
    if (child->next) {
        child->next->prev = child->prev;
    }
    child->parent = NULL;
}

// Fork current process
int process_fork(void) {
    if (!current_process) return -1;
    
    // Create new process
    process_t *child = process_create();
    if (!child) return -1;
    
    // Copy memory space
    // TODO: implement copy-on-write for efficiency
    // For now, just copy all memory regions
    
    // Copy file descriptor table
    // TODO: implement fd copying
    
    // Copy signal handlers
    for (int i = 0; i < 32; i++) {
        child->signal_handlers[i] = current_process->signal_handlers[i];
    }
    
    // Create main thread for child
    thread_t *child_thread = thread_create(child, NULL, NULL);
    if (!child_thread) {
        process_destroy(child);
        return -1;
    }
    
    // Copy execution context
    child_thread->rip = current_thread->rip;
    child_thread->rbx = current_thread->rbx;
    child_thread->rbp = current_thread->rbp;
    child_thread->r12 = current_thread->r12;
    child_thread->r13 = current_thread->r13;
    child_thread->r14 = current_thread->r14;
    child_thread->r15 = current_thread->r15;
    
    // Set return value for child (0)
    child_thread->rsp = current_thread->rsp;
    // TODO: set child's return register to 0
    
    // Add child thread to ready queue
    scheduler_add_thread(child_thread);
    
    // Return child PID to parent
    return child->pid;
}

// Execute a new program
int process_execve(process_t *proc, const char *path, char *const argv[], char *const envp[]) {
    if (!proc || !path) return -1;
    
    // TODO: load ELF executable from file
    // TODO: setup new stack with argv/envp
    // TODO: replace memory space
    // TODO: set new entry point
    
    return -1;  // Not implemented yet
}

// Wait for child process
int process_waitpid(uint32_t pid, int *status, int options) {
    if (!current_process) return -1;
    
    // Find child process
    process_t *child = current_process->children;
    while (child) {
        if (child->pid == pid || pid == -1) {
            // Wait for child to terminate
            while (child->state != PROC_STATE_ZOMBIE) {
                thread_sleep(10);  // Sleep for 10ms
            }
            
            if (status) {
                *status = child->exit_code;
            }
            
            // Remove child and destroy it
            process_remove_child(current_process, child);
            process_destroy(child);
            
            return child->pid;
        }
        child = child->next;
    }
    
    return -1;  // Child not found
}

// Exit process
void process_exit(process_t *proc, int status) {
    if (!proc) return;
    
    proc->state = PROC_STATE_ZOMBIE;
    proc->exit_code = status;
    
    // Send SIGCHLD to parent
    if (proc->parent) {
        process_send_signal(proc->parent, SIGCHLD);
    }
    
    // If this is the current process, switch to another
    if (proc == current_process) {
        scheduler_schedule();
    }
}

// Send signal to process
void process_send_signal(process_t *proc, int signum) {
    if (!proc || signum < 1 || signum > 31) return;
    
    proc->pending_signals |= (1 << (signum - 1));
    
    // If process is running, handle signal immediately
    if (proc->state == PROC_STATE_RUNNING) {
        process_handle_signals(proc);
    }
}

// Handle pending signals
void process_handle_signals(process_t *proc) {
    if (!proc) return;
    
    while (proc->pending_signals) {
        int signum = __builtin_ctz(proc->pending_signals) + 1;
        proc->pending_signals &= ~(1 << (signum - 1));
        
        void (*handler)(int) = proc->signal_handlers[signum];
        if (handler) {
            handler(signum);
        } else {
            // Default signal handling
            switch (signum) {
                case SIGTERM:
                case SIGINT:
                    process_exit(proc, signum);
                    break;
                case SIGKILL:
                    proc->state = PROC_STATE_TERMINATED;
                    break;
                case SIGSTOP:
                    proc->state = PROC_STATE_SLEEPING;
                    break;
                case SIGCONT:
                    if (proc->state == PROC_STATE_SLEEPING) {
                        proc->state = PROC_STATE_RUNNING;
                    }
                    break;
                default:
                    break;
            }
        }
    }
}

// Create a new thread
thread_t *thread_create(process_t *proc, void *(*start_routine)(void *), void *arg) {
    if (!proc) return NULL;
    
    thread_t *thread = kmalloc(sizeof(thread_t));
    if (!thread) return NULL;
    
    // Initialize thread structure
    memset(thread, 0, sizeof(thread_t));
    thread->tid = next_tid++;
    thread->pid = proc->pid;
    thread->state = THREAD_STATE_READY;
    thread->priority = 5;  // Default priority
    thread->time_slice = 10;  // 10ms time slice
    
    // Allocate stack
    thread->stack_base = kmalloc(0x10000);  // 64KB stack
    if (!thread->stack_base) {
        kfree(thread);
        return NULL;
    }
    thread->stack_top = (void*)((uint64_t)thread->stack_base + 0x10000);
    thread->rsp = (uint64_t)thread->stack_top - 8;
    
    if (start_routine) {
        // Set up thread entry point
        thread->rip = (uint64_t)thread_entry_wrapper;
        
        // Push arguments on stack
        uint64_t *stack = (uint64_t*)thread->rsp;
        *--stack = (uint64_t)arg;
        *--stack = (uint64_t)start_routine;
        *--stack = (uint64_t)thread_exit;  // Return address
        thread->rsp = (uint64_t)stack;
    }
    
    return thread;
}

// Destroy a thread
void thread_destroy(thread_t *thread) {
    if (!thread) return;
    
    // Free stack
    if (thread->stack_base) {
        kfree(thread->stack_base);
    }
    
    kfree(thread);
}

// Thread entry wrapper
void thread_entry_wrapper(void *(*start_routine)(void *), void *arg) {
    void *result = start_routine(arg);
    thread_exit(result);
}

// Exit thread
void thread_exit(void *retval) {
    if (!current_thread) return;
    
    current_thread->state = THREAD_STATE_TERMINATED;
    scheduler_remove_thread(current_thread);
    thread_destroy(current_thread);
    
    // Schedule next thread
    scheduler_schedule();
}

// Yield CPU to another thread
void thread_yield(void) {
    if (current_thread) {
        current_thread->state = THREAD_STATE_READY;
    }
    scheduler_schedule();
}

// Sleep for specified milliseconds
void thread_sleep(uint64_t milliseconds) {
    if (!current_thread) return;
    
    current_thread->state = THREAD_STATE_BLOCKED;
    // TODO: implement proper sleep timing
    scheduler_schedule();
}

// Block thread
void thread_block(thread_t *thread) {
    if (!thread) return;
    
    thread->state = THREAD_STATE_BLOCKED;
    scheduler_remove_thread(thread);
}

// Unblock thread
void thread_unblock(thread_t *thread) {
    if (!thread) return;
    
    thread->state = THREAD_STATE_READY;
    scheduler_add_thread(thread);
}

// Set thread priority
void thread_set_priority(thread_t *thread, uint8_t priority) {
    if (!thread) return;
    
    thread->priority = priority;
}

// Initialize scheduler
void scheduler_init(void) {
    ready_queue = NULL;
    current_thread = NULL;
}

// Start scheduler
void scheduler_start(void) {
    // Create idle thread if no threads exist
    if (!ready_queue) {
        // TODO: create idle thread
    }
    
    // Start scheduling
    scheduler_schedule();
}

// Add thread to ready queue
void scheduler_add_thread(thread_t *thread) {
    if (!thread) return;
    
    thread->next = ready_queue;
    thread->prev = NULL;
    
    if (ready_queue) {
        ready_queue->prev = thread;
    }
    ready_queue = thread;
}

// Remove thread from ready queue
void scheduler_remove_thread(thread_t *thread) {
    if (!thread) return;
    
    if (thread->prev) {
        thread->prev->next = thread->next;
    } else {
        ready_queue = thread->next;
    }
    
    if (thread->next) {
        thread->next->prev = thread->prev;
    }
    
    thread->next = NULL;
    thread->prev = NULL;
}

// Schedule next thread
void scheduler_schedule(void) {
    thread_t *next_thread = ready_queue;
    
    if (!next_thread) {
        // No threads ready, create idle thread or halt
        asm volatile("hlt");
        return;
    }
    
    // Remove from ready queue
    scheduler_remove_thread(next_thread);
    
    // Switch to next thread
    thread_t *prev_thread = current_thread;
    current_thread = next_thread;
    
    // Update current process
    current_process = process_find_by_pid(next_thread->pid);
    if (current_process) {
        mm_switch_space(current_process->memory_space);
    }
    
    next_thread->state = THREAD_STATE_RUNNING;
    
    if (prev_thread && prev_thread != next_thread) {
        context_switch(prev_thread, next_thread);
    }
}

// Scheduler tick (called by timer interrupt)
void scheduler_tick(void) {
    if (!current_thread) return;
    
    current_thread->time_slice--;
    
    if (current_thread->time_slice <= 0) {
        // Reset time slice and yield
        current_thread->time_slice = 10;
        thread_yield();
    }
}

// Mutex functions
int mutex_init(mutex_t *mutex) {
    if (!mutex) return -1;
    
    mutex->owner = 0;
    mutex->lock_count = 0;
    mutex->wait_queue = NULL;
    mutex->initialized = true;
    
    return 0;
}

int mutex_lock(mutex_t *mutex) {
    if (!mutex || !mutex->initialized) return -1;
    
    // Try to acquire lock
    if (mutex->owner == 0) {
        mutex->owner = current_thread->tid;
        mutex->lock_count = 1;
        return 0;
    }
    
    // If already owned by this thread, increment count
    if (mutex->owner == current_thread->tid) {
        mutex->lock_count++;
        return 0;
    }
    
    // Block thread and add to wait queue
    thread_block(current_thread);
    current_thread->next = mutex->wait_queue;
    mutex->wait_queue = current_thread;
    
    // Schedule another thread
    scheduler_schedule();
    
    return 0;
}

int mutex_unlock(mutex_t *mutex) {
    if (!mutex || !mutex->initialized) return -1;
    
    if (mutex->owner != current_thread->tid) {
        return -1;  // Not owner
    }
    
    mutex->lock_count--;
    
    if (mutex->lock_count == 0) {
        // Release lock and wake next waiter
        mutex->owner = 0;
        
        if (mutex->wait_queue) {
            thread_t *next = mutex->wait_queue;
            mutex->wait_queue = next->next;
            
            mutex->owner = next->tid;
            mutex->lock_count = 1;
            
            thread_unblock(next);
        }
    }
    
    return 0;
}

int mutex_trylock(mutex_t *mutex) {
    if (!mutex || !mutex->initialized) return -1;
    
    if (mutex->owner == 0) {
        mutex->owner = current_thread->tid;
        mutex->lock_count = 1;
        return 0;
    }
    
    return -1;  // Lock is held
}

void mutex_destroy(mutex_t *mutex) {
    if (!mutex) return;
    
    mutex->initialized = false;
    
    // Wake all waiting threads
    thread_t *waiter = mutex->wait_queue;
    while (waiter) {
        thread_t *next = waiter->next;
        thread_unblock(waiter);
        waiter = next;
    }
}

// Find process by PID
process_t *process_find_by_pid(uint32_t pid) {
    process_t *proc = process_list;
    while (proc) {
        if (proc->pid == pid) {
            return proc;
        }
        proc = proc->next;
    }
    return NULL;
}
