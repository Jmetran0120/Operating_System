/* memory.h - Memory management header for JoshOS */

#ifndef MEMORY_H
#define MEMORY_H

/* Standard integer types */
typedef unsigned int uint32_t;

/* Initialize memory manager */
void memory_init(void);

/* Allocate memory block */
void* kmalloc(uint32_t size);

/* Free previously allocated memory */
void kfree(void* ptr);

/* Allocate and zero-initialize memory */
void* kcalloc(uint32_t num, uint32_t size);

/* Reallocate memory block */
void* krealloc(void* ptr, uint32_t new_size);

/* Get memory statistics */
void memory_get_stats(uint32_t* total, uint32_t* used, uint32_t* free);

#endif /* MEMORY_H */

