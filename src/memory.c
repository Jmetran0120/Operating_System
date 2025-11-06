/* memory.c - Memory management for JoshOS
 * 
 * Implements a simple heap allocator using a linked list of free blocks.
 * This is a basic implementation suitable for kernel use.
 */

#include "memory.h"

/* Memory block structure - forms a linked list */
typedef struct MemoryBlock {
    struct MemoryBlock* next;             /* Pointer to next block */
    uint32_t size;                        /* Size of this block (in bytes) */
    uint8_t free;                         /* 1 if free, 0 if allocated */
} MemoryBlock;

/* Heap start and end addresses */
#define HEAP_START 0x1000000              /* Start of heap at 16MB */
#define HEAP_SIZE  0x100000               /* Heap size: 1MB */
#define HEAP_END   (HEAP_START + HEAP_SIZE) /* End of heap */

/* Initial heap block */
static MemoryBlock* heap_start = (MemoryBlock*)HEAP_START;

/* Initialize memory manager */
void memory_init(void) {
    /* Initialize first block as one large free block */
    heap_start->next = NULL;              /* No next block */
    heap_start->size = HEAP_SIZE - sizeof(MemoryBlock); /* Size minus header */
    heap_start->free = 1;                 /* Mark as free */
}

/* Allocate memory block of specified size */
void* kmalloc(uint32_t size) {
    /* Align size to 4 bytes (word alignment) */
    size = (size + 3) & ~3;
    
    /* Need at least sizeof(MemoryBlock) bytes */
    if (size < sizeof(MemoryBlock)) {
        size = sizeof(MemoryBlock);       /* Minimum size */
    }
    
    /* Start from beginning of heap */
    MemoryBlock* current = heap_start;
    
    /* Search for free block large enough */
    while (current != NULL) {
        /* Check if block is free and large enough */
        if (current->free && current->size >= size) {
            /* Check if we can split this block */
            if (current->size >= size + sizeof(MemoryBlock) + 4) {
                /* Split block - create new free block after allocation */
                MemoryBlock* new_block = (MemoryBlock*)((uint8_t*)current + sizeof(MemoryBlock) + size);
                new_block->next = current->next; /* Link new block */
                new_block->size = current->size - size - sizeof(MemoryBlock);
                new_block->free = 1;      /* Mark as free */
                
                /* Update current block */
                current->next = new_block; /* Link to new block */
                current->size = size;      /* Set allocated size */
            }
            
            /* Mark block as allocated */
            current->free = 0;            /* 0 = allocated */
            
            /* Return pointer to data area (after block header) */
            return (void*)((uint8_t*)current + sizeof(MemoryBlock));
        }
        
        /* Move to next block */
        current = current->next;
    }
    
    /* No free block found - out of memory */
    return NULL;                          /* Return NULL on failure */
}

/* Free previously allocated memory */
void kfree(void* ptr) {
    /* Check for NULL pointer */
    if (ptr == NULL) {
        return;                           /* Nothing to free */
    }
    
    /* Get block header (before data area) */
    MemoryBlock* block = (MemoryBlock*)((uint8_t*)ptr - sizeof(MemoryBlock));
    
    /* Check if pointer is valid - use uintptr_t for safe pointer comparison */
    /* Cast to void* first to avoid pointer-to-int cast warning on some compilers */
    void* block_ptr = (void*)block;
    uintptr_t addr = (uintptr_t)block_ptr;
    if (addr < (uintptr_t)HEAP_START || addr >= (uintptr_t)HEAP_END) {
        return;                           /* Invalid pointer */
    }
    
    /* Mark block as free */
    block->free = 1;                      /* 1 = free */
    
    /* Try to merge with next block if it's also free */
    if (block->next != NULL && block->next->free) {
        /* Merge blocks */
        block->size += sizeof(MemoryBlock) + block->next->size;
        block->next = block->next->next;  /* Skip next block */
    }
    
    /* Try to merge with previous block if it's free */
    MemoryBlock* prev = heap_start;
    while (prev != NULL && prev->next != block) {
        prev = prev->next;                /* Find previous block */
    }
    
    if (prev != NULL && prev->free) {
        /* Merge with previous block */
        prev->size += sizeof(MemoryBlock) + block->size;
        prev->next = block->next;         /* Skip current block */
    }
}

/* Allocate and zero-initialize memory */
void* kcalloc(uint32_t num, uint32_t size) {
    /* Calculate total size */
    uint32_t total_size = num * size;
    
    /* Allocate memory */
    void* ptr = kmalloc(total_size);
    
    /* Zero out memory if allocation succeeded */
    if (ptr != NULL) {
        uint8_t* byte_ptr = (uint8_t*)ptr;
        for (uint32_t i = 0; i < total_size; i++) {
            byte_ptr[i] = 0;              /* Set each byte to 0 */
        }
    }
    
    return ptr;                           /* Return pointer */
}

/* Reallocate memory block */
void* krealloc(void* ptr, uint32_t new_size) {
    /* If NULL, same as malloc */
    if (ptr == NULL) {
        return kmalloc(new_size);         /* Allocate new block */
    }
    
    /* Get block header */
    MemoryBlock* block = (MemoryBlock*)((uint8_t*)ptr - sizeof(MemoryBlock));
    
    /* If new size is 0, same as free */
    if (new_size == 0) {
        kfree(ptr);                       /* Free block */
        return NULL;                      /* Return NULL */
    }
    
    /* If new size is same or smaller, return same pointer */
    if (block->size >= new_size) {
        return ptr;                       /* No need to reallocate */
    }
    
    /* Allocate new block */
    void* new_ptr = kmalloc(new_size);
    
    /* Copy data if allocation succeeded */
    if (new_ptr != NULL) {
        uint8_t* old_byte = (uint8_t*)ptr;
        uint8_t* new_byte = (uint8_t*)new_ptr;
        
        /* Copy old data */
        for (uint32_t i = 0; i < block->size && i < new_size; i++) {
            new_byte[i] = old_byte[i];    /* Copy each byte */
        }
        
        /* Free old block */
        kfree(ptr);
    }
    
    return new_ptr;                       /* Return new pointer */
}

/* Get memory statistics */
void memory_get_stats(uint32_t* total, uint32_t* used, uint32_t* free) {
    *total = HEAP_SIZE;                   /* Total heap size */
    *used = 0;                            /* Initialize used counter */
    *free = 0;                            /* Initialize free counter */
    
    /* Traverse all blocks */
    MemoryBlock* current = heap_start;
    while (current != NULL) {
        if (current->free) {
            *free += current->size;       /* Add to free counter */
        } else {
            *used += current->size;       /* Add to used counter */
        }
        current = current->next;          /* Move to next block */
    }
}

