#ifndef FORMAT_H
#define FORMAT_H

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Converts an integer to a null-terminated string.
 *
 * @param value The integer value to convert.
 * @param str Pointer to a buffer where the resulting C-string is stored.
 * The buffer should be large enough to contain the resulting string.
 * @param base Numerical base (radix) for the conversion (e.g., 2, 8, 10, 16).
 * @param is_signed If non-zero, value is treated as signed (prepends '-' if
 * negative).
 * @param min_width Minimum width of the output string. Padded with 'pad_char'
 * if shorter.
 * @param pad_char Character to use for padding if output is shorter than
 * min_width.
 * @return A pointer to the resulting null-terminated string (same as str).
 */
char *itoa_k(long long value, char *str, int base, int is_signed, int min_width,
             char pad_char);

/**
 * @brief Kernel-level vsnprintf: formats a string into a buffer.
 *
 * Writes formatted output to a buffer `buf` of size `size` according to the
 * format string `fmt` and the variable argument list `args`.
 * Ensures null termination if `size` > 0.
 * Supports: %c, %s, %d, %i, %u, %x, %X, %p, %%.
 * Basic width and zero-padding support for integers/pointers (e.g., %08x, %5d).
 * Does NOT currently support full precision, length modifiers (l, ll, h, hh
 * fully), or float.
 *
 * @param buf The buffer to write the formatted string to.
 * @param size The maximum size of the buffer (including the null terminator).
 * @param fmt The format string.
 * @param args The variable argument list.
 * @return The number of characters that would have been written if the buffer
 * was large enough (excluding the null terminator), or a negative
 * value if an encoding error occurs or size is 0.
 */
int kvsnprintf(char *buf, size_t size, const char *fmt, va_list args);

#endif