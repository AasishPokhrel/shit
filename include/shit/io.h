#ifndef KERNEL_IO_H
#define KERNEL_IO_H

#include <stdint.h>

/**
 * @brief Writes a byte to the specified I/O port.
 * (This is a declaration; implementation is arch-specific)
 */
static inline void outb(uint16_t port, uint8_t value);

/**
 * @brief Reads a byte from the specified I/O port.
 * (This is a declaration; implementation is arch-specific)
 */
static uint8_t inb(uint16_t port);

/**
 * @brief A short I/O delay.
 * (This is a declaration; implementation is arch-specific)
 */
static void io_wait(void);

#if defined(ARCH_X86_64)
#include <port_io.h>
#else
#error "Unsupported architecture for I/O operations"
#endif

#endif