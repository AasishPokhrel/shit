#ifndef SHIT_MEM_PMM_H
#define SHIT_MEM_PMM_H

#include <limine.h>
#include <stddef.h>

#define PAGE_SIZE 0x1000
#define PAGE_SHIFT 12

/**
 * @brief Initializes the Physical Memory Manager.
 *
 * This function should be called once during kernel initialization,
 * after the Limine memory map is available. It will parse the memory map,
 * identify usable physical memory regions, and set up the page frame allocator.
 *
 * @param memmap_request Pointer to the Limine memmap request structure,
 * whose response contains the memory map.
 */
void pmm_init(volatile struct limine_memmap_request *memmap_request);

/**
 * @brief Allocates a single physical page frame.
 *
 * Finds a free page frame, marks it as used, and returns its physical address.
 *
 * @return Physical address of the allocated page frame, or 0 (NULL equivalent
 * for physical addresses) if no free page frame is available.
 */
void *pmm_alloc_page(void);

/**
 * @brief Allocates multiple contiguous physical page frames.
 *
 * @param num_pages The number of contiguous pages to allocate.
 * @return Physical address of the first allocated page frame, or 0 if
 * not enough contiguous pages are available.
 */
void *pmm_alloc_contiguous_pages(size_t num_pages);

/**
 * @brief Frees a previously allocated physical page frame.
 *
 * Marks the page frame at the given physical address as free.
 *
 * @param p_addr Physical address of the page frame to free.
 */
void pmm_free_page(void *p_addr);

/**
 * @brief Frees multiple contiguous physical page frames.
 *
 * @param p_addr Physical address of the first page frame in the contiguous
 * block.
 * @param num_pages The number of contiguous pages to free.
 */
void pmm_free_contiguous_pages(void *p_addr, size_t num_pages);

/**
 * @brief Gets the total amount of usable physical memory.
 * @return Total usable memory in bytes.
 */
uint64_t pmm_get_total_memory(void);

/**
 * @brief Gets the amount of free physical memory.
 * @return Free memory in bytes.
 */
uint64_t pmm_get_free_memory(void);

/**
 * @brief Gets the amount of used physical memory.
 * @return Used memory in bytes.
 */
uint64_t pmm_get_used_memory(void);

#endif // SHIT_MEM_PMM_H