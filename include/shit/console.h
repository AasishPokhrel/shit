#ifndef SHIT_CONSOLE_H
#define SHIT_CONSOLE_H

#include <stddef.h>
#include <stdint.h>

/**
 * This is the main public header for Nucleus's console.
 */

// Let's set up some default colors for our console.
// The 0x00 at the start is for Alpha.
#define CONSOLE_DEFAULT_FG_COLOR 0x00D0D0D0
#define CONSOLE_DEFAULT_BG_COLOR 0x00000000

/**
 * @brief Initializes the console system.
 *
 * It gets the framebuffer ready, loads our font,
 * figures out how many rows/columns we have and gives the screen a fresh wipe.
 */
void console_init(void);

/**
 * @brief Wipes the console.
 *
 * This function will fill the entire screen with the current default background
 * color and send the cursor back to the top-left corner (0,0).
 */
void console_clear(void);

/**
 * @brief Writes a single character onto the screen.
 *
 * This is for printing one character at a time. It'll use the
 * default colors.
 *
 * @param c The character you want to put
 */
void console_putchar(char c);

/**
 * @brief Prints a whole string of characters using default colors.
 *
 * Give it a null-terminated string and it'll print it out character by
 * character using console_putchar.
 *
 * @param str The string of text you want to display
 */
void console_writestring(const char *str);

#endif // SHIT_CONSOLE_H