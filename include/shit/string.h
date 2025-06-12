#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Calculates the length of a null-terminated string.
 *
 * Counts the number of characters in the string pointed to by `s`
 * before the terminating null byte.
 *
 * @param s The null-terminated string.
 * @return The number of characters in the string (excluding the null
 * terminator).
 */
size_t strlen(const char *s);

/**
 * @brief Copies a null-terminated string.
 *
 * Copies the string pointed to by `src` (including the terminating null byte)
 * to the buffer pointed to by `dest`. The destination buffer must be large
 * enough to hold the source string.
 * Behavior is undefined if the source and destination strings overlap.
 *
 * @param dest Pointer to the destination array where the content is to be
 * copied.
 * @param src Pointer to the null-terminated string to be copied.
 * @return A pointer to the destination string `dest`.
 */
char *strcpy(char *dest, const char *src);

/**
 * @brief Copies a specified number of characters from a string.
 *
 * Copies up to `n` characters from the string pointed to by `src` to `dest`.
 * If the length of `src` is less than `n`, the remainder of `dest` up to `n`
 * characters is filled with null bytes. If `src` is `n` characters or longer,
 * `dest` will NOT be null-terminated.
 * Behavior is undefined if the source and destination strings overlap.
 *
 * @param dest Pointer to the destination array.
 * @param src Pointer to the source string.
 * @param n Maximum number of characters to be copied from `src`.
 * @return A pointer to the destination string `dest`.
 */
char *strncpy(char *dest, const char *src, size_t n);

/**
 * @brief Concatenates two strings.
 *
 * Appends a copy of the null-terminated string `src` to the end of the
 * null-terminated string `dest`. The `dest` string must have enough space
 * for the result. The terminating null byte in `dest` is overwritten by the
 * first character of `src`, and a new null byte is appended at the end.
 * Behavior is undefined if the source and destination strings overlap.
 *
 * @param dest The destination string.
 * @param src The source string to append.
 * @return A pointer to the destination string `dest`.
 */
char *strcat(char *dest, const char *src);

/**
 * @brief Concatenates a specified number of characters from a string.
 *
 * Appends at most `n` characters from the null-terminated string `src` to
 * the end of the null-terminated string `dest`, plus a terminating null byte.
 * If `src` contains `n` or more characters, `n` characters are appended, and
 * a null terminator is added. If `src` is shorter than `n`, all of `src` is
 * appended, plus a null terminator.
 * Behavior is undefined if the source and destination strings overlap.
 *
 * @param dest The destination string.
 * @param src The source string to append.
 * @param n Maximum number of characters to append from `src`.
 * @return A pointer to the destination string `dest`.
 */
char *strncat(char *dest, const char *src, size_t n);

/**
 * @brief Compares two null-terminated strings.
 *
 * Compares string `s1` to string `s2`.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 * @return An integer less than, equal to, or greater than zero if `s1` is
 * found, respectively, to be less than, to match, or be greater than `s2`.
 */
int strcmp(const char *s1, const char *s2);

/**
 * @brief Compares a specified number of characters of two strings.
 *
 * Compares at most `n` characters of string `s1` to string `s2`.
 *
 * @param s1 The first string.
 * @param s2 The second string.
 * @param n Maximum number of characters to compare.
 * @return An integer less than, equal to, or greater than zero if the first `n`
 * bytes (or fewer if a null byte is encountered) of `s1` is found,
 * respectively, to be less than, to match, or be greater than `s2`.
 */
int strncmp(const char *s1, const char *s2, size_t n);

/**
 * @brief Copies a block of memory.
 *
 * Copies `n` bytes from memory area `src` to memory area `dest`.
 * The memory areas must not overlap. Use memmove if they might overlap.
 *
 * @param dest Pointer to the destination array.
 * @param src Pointer to the source of data to be copied.
 * @param n Number of bytes to copy.
 * @return A pointer to the destination `dest`.
 */
void *memcpy(void *dest, const void *src, size_t n);

/**
 * @brief Fills a block of memory with a specific value.
 *
 * Sets the first `n` bytes of the block of memory pointed by `s`
 * to the specified value `c` (interpreted as an unsigned char).
 *
 * @param s Pointer to the block of memory to fill.
 * @param c Value to be set. The value is passed as an int, but the function
 * fills the block of memory using the unsigned char conversion of this value.
 * @param n Number of bytes to be set to the value.
 * @return A pointer to the memory area `s`.
 */
void *memset(void *s, int c, size_t n);

/**
 * @brief Compares two blocks of memory.
 *
 * Compares the first `n` bytes of memory area `s1` and memory area `s2`.
 *
 * @param s1 Pointer to the first block of memory.
 * @param s2 Pointer to the second block of memory.
 * @param n Number of bytes to compare.
 * @return An integer less than, equal to, or greater than zero if the first `n`
 * bytes of `s1` is found, respectively, to be less than, to match,
 * or be greater than `s2`.
 */
int memcmp(const void *s1, const void *s2, size_t n);

/**
 * @brief Moves a block of memory.
 *
 * Copies `n` bytes from memory area `src` to memory area `dest`.
 * The memory areas may overlap: copying takes place as if the bytes were
 * first copied into a temporary array and then copied from the temporary
 * array to `dest`.
 *
 * @param dest Pointer to the destination array.
 * @param src Pointer to the source of data to be copied.
 * @param n Number of bytes to copy.
 * @return A pointer to the destination `dest`.
 */
void *memmove(void *dest, const void *src, size_t n);

/**
 * @brief Locates the first occurrence of a character in a string.
 *
 * Searches for the first occurrence of the character `c` (an unsigned char)
 * in the string pointed to by `s`. The terminating null byte is considered
 * part of the string.
 *
 * @param s The string to be scanned.
 * @param c The character to be searched in `s`.
 * @return A pointer to the first occurrence of the character `c` in `s`,
 * or NULL if the character is not found.
 */
char *strchr(const char *s, int c);

#endif // STRING_H