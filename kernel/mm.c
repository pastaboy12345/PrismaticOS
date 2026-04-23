#include "mm.h"
#include "kernel.h"
#include <string.h>

// Physical memory bitmap
static uint8_t *memory_bitmap;
static size_t memory_bitmap_size;
static uint64_t memory_total_pages;
static uint64_t memory_used_pages;
static uint64_t memory_base;

// Kernel heap
static void *kernel_heap_start;
static void *kernel_heap_end;
static void *kernel_heap_current;

// Page directory
static uint64_t kernel_pml4[512] __attribute__((aligned(4096)));

// Memory bitmap operations
static inline void bitmap_set_bit(uint64_t bit) {
    memory_bitmap[bit / 8] |= (1 << (bit % 8));
}

static inline void bitmap_clear_bit(uint64_t bit) {
    memory_bitmap[bit / 8] &= ~(1 << (bit % 8));
}

static inline bool bitmap_test_bit(uint64_t bit) {
    return memory_bitmap[bit / 8] & (1 << (bit % 8));
}

static uint64_t bitmap_find_free_page(void) {
    for (uint64_t i = 0; i < memory_total_pages; i++) {
        if (!bitmap_test_bit(i)) {
            return i;
        }
    }
    return (uint64_t)-1;
}

// Physical memory management
void pm_init_region(uint64_t start, size_t size) {
    memory_base = start;
    memory_total_pages = size / PAGE_SIZE;
    memory_bitmap_size = (memory_total_pages + 7) / 8;
    
    // Allocate bitmap in kernel memory
    memory_bitmap = (uint8_t*)kmalloc(memory_bitmap_size);
    if (!memory_bitmap) {
        panic("Failed to allocate memory bitmap");
    }
    
    // Mark all pages as free
    memset(memory_bitmap, 0, memory_bitmap_size);
    
    // Mark kernel pages as used
    uint64_t kernel_end = (uint64_t)kernel_heap_end;
    uint64_t kernel_pages = PAGE_ALIGN(kernel_end) / PAGE_SIZE;
    
    for (uint64_t i = 0; i < kernel_pages && i < memory_total_pages; i++) {
        bitmap_set_bit(i);
        memory_used_pages++;
    }
}

uint64_t pm_alloc_page(void) {
    uint64_t page_index = bitmap_find_free_page();
    if (page_index == (uint64_t)-1) {
        return 0;  // Out of memory
    }
    
    bitmap_set_bit(page_index);
    memory_used_pages++;
    
    return memory_base + (page_index * PAGE_SIZE);
}

void pm_free_page(uint64_t phys_addr) {
    uint64_t page_index = (phys_addr - memory_base) / PAGE_SIZE;
    
    if (page_index < memory_total_pages && bitmap_test_bit(page_index)) {
        bitmap_clear_bit(page_index);
        memory_used_pages--;
    }
}

// Virtual memory management
static void init_page_tables(void) {
    // Identity map the first 2GB
    for (uint64_t i = 0; i < 512; i++) {
        uint64_t *pml4_entry = &kernel_pml4[i];
        if (i == 0) {  // First PML4 entry
            uint64_t *pdpt = (uint64_t*)kmalloc(PAGE_SIZE);
            memset(pdpt, 0, PAGE_SIZE);
            *pml4_entry = (uint64_t)pdpt | PTE_PRESENT | PTE_WRITE;
            
            for (uint64_t j = 0; j < 512; j++) {
                uint64_t *pdpt_entry = &pdpt[j];
                if (j < 4) {  // First 4 PDPT entries (4GB)
                    uint64_t *pd = (uint64_t*)kmalloc(PAGE_SIZE);
                    memset(pd, 0, PAGE_SIZE);
                    *pdpt_entry = (uint64_t)pd | PTE_PRESENT | PTE_WRITE;
                    
                    for (uint64_t k = 0; k < 512; k++) {
                        uint64_t *pd_entry = &pd[k];
                        uint64_t phys_addr = (i * 512 * 512 * PAGE_SIZE) + 
                                           (j * 512 * PAGE_SIZE) + 
                                           (k * PAGE_SIZE);
                        *pd_entry = phys_addr | PTE_PRESENT | PTE_WRITE | PTE_GLOBAL;
                    }
                }
            }
        }
    }
    
    // Enable paging
    asm volatile(
        "mov %0, %%cr3\n"
        "mov %%cr4, %%rax\n"
        "or $0x10, %%rax\n"
        "mov %%rax, %%cr4\n"
        "mov %%cr0, %%rax\n"
        "or $0x80000001, %%rax\n"
        "mov %%rax, %%cr0"
        : : "r" ((uint64_t)kernel_pml4) : "rax"
    );
}

memory_space_t *mm_create_space(void) {
    memory_space_t *space = kmalloc(sizeof(memory_space_t));
    if (!space) return NULL;
    
    // Allocate new page table
    space->pml4 = (uint64_t)kmalloc(PAGE_SIZE);
    if (!space->pml4) {
        kfree(space);
        return NULL;
    }
    
    // Copy kernel mappings
    uint64_t *new_pml4 = (uint64_t*)space->pml4;
    uint64_t *kernel_pml4_ptr = kernel_pml4;
    
    // Copy first 256 entries (kernel space)
    for (int i = 0; i < 256; i++) {
        new_pml4[i] = kernel_pml4_ptr[i];
    }
    
    // Clear user space entries
    for (int i = 256; i < 512; i++) {
        new_pml4[i] = 0;
    }
    
    space->regions = NULL;
    space->heap_start = (void*)0x100000000ULL;  // Start at 4GB
    space->heap_end = space->heap_start;
    space->stack_start = (void*)0x7FFFFF000000ULL;  // High memory for stack
    space->stack_end = space->stack_start;
    
    return space;
}

void mm_destroy_space(memory_space_t *space) {
    // Free all memory regions
    memory_region_t *region = space->regions;
    while (region) {
        memory_region_t *next = region->next;
        
        // Unmap pages
        for (uint64_t addr = region->start; addr < region->end; addr += PAGE_SIZE) {
            mm_unmap_page(space, (void*)addr);
        }
        
        kfree(region);
        region = next;
    }
    
    // Free page table
    kfree((void*)space->pml4);
    kfree(space);
}

void *mm_map_page(memory_space_t *space, void *virt_addr, uint64_t phys_addr, uint32_t flags) {
    uint64_t *pml4 = (uint64_t*)space->pml4;
    uint64_t pml4_index = ((uint64_t)virt_addr >> 39) & 0x1FF;
    uint64_t pdpt_index = ((uint64_t)virt_addr >> 30) & 0x1FF;
    uint64_t pd_index = ((uint64_t)virt_addr >> 21) & 0x1FF;
    uint64_t pt_index = ((uint64_t)virt_addr >> 12) & 0x1FF;
    
    // Ensure PML4 entry exists
    if (!(pml4[pml4_index] & PTE_PRESENT)) {
        uint64_t *pdpt = kmalloc(PAGE_SIZE);
        memset(pdpt, 0, PAGE_SIZE);
        pml4[pml4_index] = (uint64_t)pdpt | PTE_PRESENT | PTE_WRITE | PTE_USER;
    }
    
    uint64_t *pdpt = (uint64_t*)(pml4[pml4_index] & ~0xFFFULL);
    
    // Ensure PDPT entry exists
    if (!(pdpt[pdpt_index] & PTE_PRESENT)) {
        uint64_t *pd = kmalloc(PAGE_SIZE);
        memset(pd, 0, PAGE_SIZE);
        pdpt[pdpt_index] = (uint64_t)pd | PTE_PRESENT | PTE_WRITE | PTE_USER;
    }
    
    uint64_t *pd = (uint64_t*)(pdpt[pdpt_index] & ~0xFFFULL);
    
    // Ensure PD entry exists
    if (!(pd[pd_index] & PTE_PRESENT)) {
        uint64_t *pt = kmalloc(PAGE_SIZE);
        memset(pt, 0, PAGE_SIZE);
        pd[pd_index] = (uint64_t)pt | PTE_PRESENT | PTE_WRITE | PTE_USER;
    }
    
    uint64_t *pt = (uint64_t*)(pd[pd_index] & ~0xFFFULL);
    
    // Map the page
    uint64_t entry = phys_addr | PTE_PRESENT | PTE_USER;
    
    if (flags & MEM_WRITE) entry |= PTE_WRITE;
    if (!(flags & MEM_EXEC)) entry |= PTE_NX;
    
    pt[pt_index] = entry;
    
    // Invalidate TLB
    __asm__ volatile("invlpg (%0)" : : "r" (virt_addr) : "memory");
    
    return virt_addr;
}

void mm_unmap_page(memory_space_t *space, void *virt_addr) {
    uint64_t *pml4 = (uint64_t*)space->pml4;
    uint64_t pml4_index = ((uint64_t)virt_addr >> 39) & 0x1FF;
    uint64_t pdpt_index = ((uint64_t)virt_addr >> 30) & 0x1FF;
    uint64_t pd_index = ((uint64_t)virt_addr >> 21) & 0x1FF;
    uint64_t pt_index = ((uint64_t)virt_addr >> 12) & 0x1FF;
    
    if (!(pml4[pml4_index] & PTE_PRESENT)) return;
    
    uint64_t *pdpt = (uint64_t*)(pml4[pml4_index] & ~0xFFFULL);
    if (!(pdpt[pdpt_index] & PTE_PRESENT)) return;
    
    uint64_t *pd = (uint64_t*)(pdpt[pdpt_index] & ~0xFFFULL);
    if (!(pd[pd_index] & PTE_PRESENT)) return;
    
    uint64_t *pt = (uint64_t*)(pd[pd_index] & ~0xFFFULL);
    
    // Unmap the page
    pt[pt_index] = 0;
    
    // Invalidate TLB
    __asm__ volatile("invlpg (%0)" : : "r" (virt_addr) : "memory");
}

void *mm_alloc_region(memory_space_t *space, size_t size, uint32_t flags) {
    size = PAGE_ALIGN(size);
    
    // Find a free region in the address space
    void *addr = space->heap_end;
    
    // Check if this region conflicts with existing regions
    memory_region_t *region = space->regions;
    while (region) {
        if ((uint64_t)addr >= region->start && (uint64_t)addr < region->end) {
            addr = (void*)region->end;
            region = space->regions;  // Restart search
            continue;
        }
        region = region->next;
    }
    
    // Allocate physical pages and map them
    for (size_t i = 0; i < size; i += PAGE_SIZE) {
        uint64_t phys_addr = pm_alloc_page();
        if (!phys_addr) {
            // Rollback
            for (size_t j = 0; j < i; j += PAGE_SIZE) {
                uint64_t old_phys = 0;  // Would need to track this
                pm_free_page(old_phys);
                mm_unmap_page(space, (void*)((uint64_t)addr + j));
            }
            return NULL;
        }
        
        mm_map_page(space, (void*)((uint64_t)addr + i), phys_addr, flags);
    }
    
    // Create memory region
    memory_region_t *new_region = kmalloc(sizeof(memory_region_t));
    new_region->start = (uint64_t)addr;
    new_region->end = (uint64_t)addr + size;
    new_region->flags = flags;
    strcpy(new_region->name, "user_region");
    new_region->next = space->regions;
    space->regions = new_region;
    
    // Update heap pointer
    space->heap_end = (void*)((uint64_t)addr + size);
    
    return addr;
}

void mm_free_region(memory_space_t *space, void *addr, size_t size) {
    // Find and remove the memory region
    memory_region_t *prev = NULL;
    memory_region_t *region = space->regions;
    
    while (region) {
        if (region->start == (uint64_t)addr && region->end == (uint64_t)addr + size) {
            if (prev) {
                prev->next = region->next;
            } else {
                space->regions = region->next;
            }
            
            // Unmap pages
            for (uint64_t i = region->start; i < region->end; i += PAGE_SIZE) {
                mm_unmap_page(space, (void*)i);
            }
            
            kfree(region);
            return;
        }
        prev = region;
        region = region->next;
    }
}

// Kernel heap management (simple bump allocator)
struct heap_block {
    size_t size;
    bool used;
    struct heap_block *next;
};

static struct heap_block *heap_free_list;

void *kmalloc(size_t size) {
    if (size == 0) return NULL;
    
    // Align size to 8 bytes
    size = (size + 7) & ~7;
    
    // Look for free block
    struct heap_block *block = heap_free_list;
    struct heap_block *prev __attribute__((unused)) = NULL;
    
    while (block) {
        if (!block->used && block->size >= size) {
            block->used = true;
            
            // Split block if too large
            if (block->size > size + sizeof(struct heap_block)) {
                struct heap_block *new_block = (struct heap_block*)((uint8_t*)block + sizeof(struct heap_block) + size);
                new_block->size = block->size - size - sizeof(struct heap_block);
                new_block->used = false;
                new_block->next = block->next;
                block->size = size;
                block->next = new_block;
            }
            
            return (void*)((uint8_t*)block + sizeof(struct heap_block));
        }
        prev = block;
        block = block->next;
    }
    
    // No suitable block found, allocate from heap
    if (kernel_heap_current + size + sizeof(struct heap_block) > kernel_heap_end) {
        return NULL;  // Out of memory
    }
    
    block = (struct heap_block*)kernel_heap_current;
    block->size = size;
    block->used = true;
    block->next = heap_free_list;
    heap_free_list = block;
    
    kernel_heap_current += size + sizeof(struct heap_block);
    
    return (void*)((uint8_t*)block + sizeof(struct heap_block));
}

void kfree(void *ptr) {
    if (!ptr) return;
    
    struct heap_block *block = (struct heap_block*)((uint8_t*)ptr - sizeof(struct heap_block));
    block->used = false;
    
    // Try to merge with next block
    if (block->next && !block->next->used) {
        block->size += block->next->size + sizeof(struct heap_block);
        block->next = block->next->next;
    }
}

void *krealloc(void *ptr, size_t new_size) {
    if (!ptr) return kmalloc(new_size);
    if (new_size == 0) {
        kfree(ptr);
        return NULL;
    }
    
    struct heap_block *block = (struct heap_block*)((uint8_t*)ptr - sizeof(struct heap_block));
    
    if (block->size >= new_size) {
        return ptr;  // Current block is large enough
    }
    
    // Allocate new block
    void *new_ptr = kmalloc(new_size);
    if (!new_ptr) return NULL;
    
    // Copy data
    size_t copy_size = block->size < new_size ? block->size : new_size;
    memcpy(new_ptr, ptr, copy_size);
    
    // Free old block
    kfree(ptr);
    
    return new_ptr;
}

// Initialize memory management
void mm_init(void) {
    // Set up kernel heap (starts after kernel image)
    extern uint8_t kernel_end[];
    kernel_heap_start = kernel_heap_current = (void*)PAGE_ALIGN((uint32_t)kernel_end);
    kernel_heap_end = (void*)(kernel_heap_start + 0x100000);  // 1MB initial heap
    
    // Initialize page tables
    init_page_tables();
    
    // Initialize physical memory (assume 256MB for now)
    pm_init_region(0x100000, 0x10000000);  // 256MB starting at 1MB
}

size_t mm_get_available_memory(void) {
    return (memory_total_pages - memory_used_pages) * PAGE_SIZE;
}
