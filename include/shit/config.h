#ifndef KERNEL_CONFIG_H
#define KERNEL_CONFIG_H

#define KERNEL_VIRTUAL_BASE       0xffffffff80000000ULL
#define KERNEL_PHYSICAL_LOAD_ADDR 0x0000000000100000ULL
#define PAGE_SIZE                 0x1000

extern char _kernel_end[];

#endif // KERNEL_CONFIG_H