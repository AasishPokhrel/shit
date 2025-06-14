#include <shit/memory/kheap.h>
#include <shit/memory/pmm.h>
#include <shit/printk.h>
#include <shit/string.h>
#include <stdbool.h>
#include <stddef.h>

#define HEAP_ALIGNMENT sizeof(void *)

static inline size_t align_up(size_t size, size_t alignment) {
  return (size + alignment - 1) & ~(alignment - 1);
}

typedef struct heap_block_header {
  size_t size;
  bool is_free;
  struct heap_block_header *next_free;
  struct heap_block_header *prev_phys;
  struct heap_block_header *next_phys;
} heap_block_header_t;

#define HEADER_SIZE align_up(sizeof(heap_block_header_t), HEAP_ALIGNMENT)

static heap_block_header_t *g_free_list_head = NULL;

static uintptr_t g_heap_start_addr = 0;
static uintptr_t g_heap_end_addr = 0;
static size_t g_heap_total_size = 0;

int kheap_init(void *initial_pool_start, size_t initial_pool_size) {
  printk(KERN_INFO "KHEAP: Initializing kernel heap...\n");

  if (!initial_pool_start ||
      initial_pool_size < (HEADER_SIZE + HEAP_ALIGNMENT)) {
    printk(KERN_ERR "KHEAP: Initial pool is too small or NULL.\n");
    return -1;
  }

  g_heap_start_addr = align_up((uintptr_t)initial_pool_start, PAGE_SIZE);
  uintptr_t actual_pool_start_after_align = (uintptr_t)initial_pool_start;
  size_t alignment_diff = g_heap_start_addr - actual_pool_start_after_align;

  if (alignment_diff > initial_pool_size ||
      initial_pool_size - alignment_diff < (HEADER_SIZE + HEAP_ALIGNMENT)) {
    printk(KERN_ERR
           "KHEAP: Initial pool too small after aligning start address.\n");
    g_heap_start_addr = 0;
    return -1;
  }

  g_heap_total_size = initial_pool_size - alignment_diff;
  g_heap_end_addr = g_heap_start_addr + g_heap_total_size;

  g_free_list_head = (heap_block_header_t *)g_heap_start_addr;
  g_free_list_head->size = g_heap_total_size - HEADER_SIZE;
  g_free_list_head->is_free = true;
  g_free_list_head->next_free = NULL;
  g_free_list_head->prev_phys = NULL;
  g_free_list_head->next_phys = NULL;

  printk(KERN_INFO
         "KHEAP: Heap initialized. Start: 0x%p, End: 0x%p, Size: %lu bytes.\n",
         (void *)g_heap_start_addr, (void *)g_heap_end_addr, g_heap_total_size);
  printk(KERN_DEBUG "KHEAP: First free block created. Size: %lu bytes.\n",
         g_free_list_head->size);

  return 0;
}

void *kmalloc(size_t size) {
  if (size == 0) {
    return NULL;
  }

  if (!g_free_list_head) {
    printk(KERN_WARN "KHEAP: kmalloc called but heap not initialized or no "
                     "free blocks!\n");
    return NULL;
  }

  size_t actual_data_size = align_up(size, HEAP_ALIGNMENT);

  heap_block_header_t *current = g_free_list_head;
  heap_block_header_t *prev_free = NULL;

  while (current) {
    if (current->is_free && current->size >= actual_data_size) {
      if (current->size >= actual_data_size + HEADER_SIZE + HEAP_ALIGNMENT) {
        heap_block_header_t *remaining_free_header =
            (heap_block_header_t *)((uintptr_t)current + HEADER_SIZE +
                                    actual_data_size);

        remaining_free_header->size =
            current->size - actual_data_size - HEADER_SIZE;
        remaining_free_header->is_free = true;
        remaining_free_header->next_free = current->next_free;

        remaining_free_header->prev_phys = current;
        remaining_free_header->next_phys = current->next_phys;
        if (current->next_phys) {
          current->next_phys->prev_phys = remaining_free_header;
        }
        current->next_phys = remaining_free_header;

        current->size = actual_data_size;
        current->is_free = false;

        if (prev_free) {
          prev_free->next_free = remaining_free_header;
        } else {
          g_free_list_head = remaining_free_header;
        }
      } else {
        current->is_free = false;
        if (prev_free) {
          prev_free->next_free = current->next_free;
        } else {
          g_free_list_head = current->next_free;
        }
      }
    }

    void *user_ptr = (void *)((uintptr_t)current + HEADER_SIZE);

    return user_ptr;
  }

  prev_free = current;
  current = current->next_free;

  printk(KERN_WARN "KHEAP: kmalloc failed to find a suitable free block for "
                   "size %u (aligned %u).\n",
         size, actual_data_size);

  // TODO: Implement heap expansion by requesting more pages from PMM if we
  // run out.
  return NULL;
}

void kfree(void *ptr) {
  if (!ptr) {
    return; // Nothing to free!
  }

  // spinlock_lock(&kheap_lock);

  // Get the header from the user pointer
  heap_block_header_t *block_header =
      (heap_block_header_t *)((uintptr_t)ptr - HEADER_SIZE);

  // Some sanity checks (optional, but good for debugging)
  if (block_header->is_free) {
    printk(KERN_ERR "KHEAP: Double free detected at %p (header %p)!\n", ptr,
           block_header);
    // spinlock_unlock(&kheap_lock);
    // PANIC("Double free!");
    return;
  }
  // Check if ptr is within heap bounds (g_heap_start_addr, g_heap_end_addr)
  // Check magic number if using one

  block_header->is_free = true;

  // --- Simple Coalescing (merge with adjacent free blocks) ---
  // This is a basic implementation. More robust coalescing would check both
  // prev_phys and next_phys and update the free list carefully. For now,
  // we'll just add it to the head of the free list. A better approach would
  // be to keep the free list sorted by address for easier coalescing.

  // Try to coalesce with next physical block if it's free
  if (block_header->next_phys && block_header->next_phys->is_free) {
    // printk(KERN_DEBUG "KHEAP: Coalescing block at %p with next block at
    // %p\n", block_header, block_header->next_phys); Remove next_phys from
    // the free list (it's about to be merged)
    heap_block_header_t *curr_free = g_free_list_head;
    heap_block_header_t *prev_free = NULL;
    while (curr_free) {
      if (curr_free == block_header->next_phys) {
        if (prev_free)
          prev_free->next_free = curr_free->next_free;
        else
          g_free_list_head = curr_free->next_free;
        break;
      }
      prev_free = curr_free;
      curr_free = curr_free->next_free;
    }

    block_header->size += HEADER_SIZE + block_header->next_phys->size;
    block_header->next_phys = block_header->next_phys->next_phys;
    if (block_header->next_phys) {
      block_header->next_phys->prev_phys = block_header;
    }
  }

  // Try to coalesce with previous physical block if it's free
  // This requires finding block_header in the free list to remove it first if
  // it was just added, then merging with prev_phys and re-adding prev_phys.
  // For simplicity in this version, prev_phys coalescing is more complex if
  // free list isn't address-ordered. We'll skip merging with previous for now
  // to keep kfree simpler. A more advanced kfree would handle this.

  // Add the (potentially coalesced) block to the head of the free list
  block_header->next_free = g_free_list_head;
  g_free_list_head = block_header;

  // printk(KERN_DEBUG "KHEAP: Freed block at %p (header %p), new size %u\n",
  // ptr, block_header, block_header->size); spinlock_unlock(&kheap_lock);
}

void *kcalloc(size_t num, size_t element_size) {
  size_t total_size = num * element_size;
  // Check for overflow
  if (element_size != 0 && total_size / element_size != num) {
    printk(KERN_WARN "KHEAP: kcalloc multiplication overflow (num=%u, "
                     "element_size=%u)\n",
           num, element_size);
    return NULL;
  }

  void *ptr = kmalloc(total_size);
  if (ptr) {
    memset(ptr, 0, total_size);
  }
  return ptr;
}