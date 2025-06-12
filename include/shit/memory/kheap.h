#ifndef KERNEL_MEMORY_KHEAP_H
#define KERNEL_MEMORY_KHEAP_H

#include <stddef.h>

/**
 * @brief Initializes the kernel heap.
 *
 * This function needs to be called once (after PMM is initialized)
 * to set up the initial memory pool for the heap.
 *
 * @param initial_pool_start The virtual address of the start of the memory pool
 * (obtained from PMM and mapped by VMM, or via HHDM).
 * @param initial_pool_size The size of the initial memory pool in bytes.
 * @return 0 on success, non-zero on failure.
 */
int kheap_init(void *initial_pool_start, size_t initial_pool_size);

/**
 * @brief Allocates a block of memory from the kernel heap.
 *
 * Tries to find a free block of at least 'size' bytes. The actual allocated
 * block might be slightly larger to accommodate metadata.
 *
 * @param size The number of bytes to allocate.
 * @return A pointer to the beginning of the allocated memory block,
 * or NULL if allocation fails (e.g., out of memory, or no suitable block).
 */
void *kmalloc(size_t size);

/**
 * @brief Frees a previously allocated block of memory.
 *
 * Adds the block back to the free list, potentially merging with
 * adjacent free blocks.
 *
 * @param ptr Pointer to the memory block to free (must be a pointer
 * previously returned by kmalloc). Behavior is undefined
 * if ptr is invalid or already freed.
 */
void kfree(void *ptr);

/**
 * @brief Allocates memory and initializes it to zero.
 *
 * @param num Number of elements to allocate.
 * @param size Size of each element.
 * @return Pointer to allocated and zeroed memory, or NULL on failure.
 */
void *kcalloc(size_t num, size_t size);

/**
 * @brief Reallocates a memory block.
 * (More advanced, can be added later - for now, a simple stub or not included)
 * Changes the size of the memory block pointed to by ptr.
 * The contents will be unchanged up to the minimum of the old and new sizes.
 * If the new size is larger, the uninitialized new space is not guaranteed.
 * If ptr is NULL, it's like kmalloc(size).
 * If size is 0 and ptr is not NULL, it's like kfree(ptr).
 *
 * @param ptr Pointer to the memory block to reallocate.
 * @param new_size The new size for the memory block.
 * @return Pointer to the reallocated memory block (which might be different
 * from ptr), or NULL if reallocation fails (original block is unchanged).
 */
// void* krealloc(void *ptr, size_t new_size); // TODO for later

#endif // KERNEL_MEMORY_KHEAP_H