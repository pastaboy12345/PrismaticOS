#ifndef MM_H
#define MM_H

#include <stdint.h>
#include <stddef.h>

// Page size
#define PAGE_SIZE 4096
#define PAGE_ALIGN(addr) (((addr) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

// Memory protection flags
#define PROT_READ    0x1
#define PROT_WRITE   0x2
#define PROT_EXEC    0x4

// Memory mapping flags
#define MAP_SHARED    0x01
#define MAP_PRIVATE   0x02
#define MAP_FIXED     0x10
#define MAP_ANONYMOUS 0x20

// Memory region flags
#define MEM_READ      0x1
#define MEM_WRITE     0x2
#define MEM_EXEC      0x4
#define MEM_KERNEL    0x8

// Page table entry flags
#define PTE_PRESENT   0x1
#define PTE_WRITE     0x2
#define PTE_USER      0x4
#define PTE_WRITETHRU 0x8
#define PTE_NOCACHE   0x10
#define PTE_ACCESSED  0x20
#define PTE_DIRTY     0x40
#define PTE_GLOBAL    0x100
#define PTE_NX        0x8000000000000000ULL

// Memory region
typedef struct memory_region {
    uint64_t start;
    uint64_t end;
    uint32_t flags;  // read/write/execute
    char name[32];
    struct memory_region *next;
} memory_region_t;

// Process memory space
typedef struct memory_space {
    uint64_t pml4;  // Page map level 4
    memory_region_t *regions;
    void *heap_start;
    void *heap_end;
    void *stack_start;
    void *stack_end;
} memory_space_t;

// Memory management functions
void mm_init(void);
size_t mm_get_available_memory(void);
void *mm_alloc_pages(size_t count);
void mm_free_pages(void *addr, size_t count);
void *mm_alloc_dma(size_t size, uint64_t *phys_addr);
void mm_free_dma(void *virt_addr, uint64_t phys_addr);

// Virtual memory management
memory_space_t *mm_create_space(void);
void mm_destroy_space(memory_space_t *space);
void mm_switch_space(memory_space_t *space);
void *mm_map_page(memory_space_t *space, void *virt_addr, uint64_t phys_addr, uint32_t flags);
void mm_unmap_page(memory_space_t *space, void *virt_addr);
void *mm_alloc_region(memory_space_t *space, size_t size, uint32_t flags);
void mm_free_region(memory_space_t *space, void *addr, size_t size);

// Heap management
void *kmalloc(size_t size);
void kfree(void *ptr);
void *krealloc(void *ptr, size_t new_size);

// Physical memory management
uint64_t pm_alloc_page(void);
void pm_free_page(uint64_t phys_addr);
void pm_init_region(uint64_t start, size_t size);

#endif // MM_H
