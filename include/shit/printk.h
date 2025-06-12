#ifndef SHIT_PRINTK_H
#define SHIT_PRINTK_H

#include <stdarg.h>
#include <stddef.h>

#define KERN_EMERG "<0>"  /* System is unusable */
#define KERN_ALERT "<1>"  /* Action must be taken immediately */
#define KERN_CRIT "<2>"   /* Critical conditions */
#define KERN_ERR "<3>"    /* Error conditions */
#define KERN_WARN "<4>"   /* Warning conditions */
#define KERN_NOTICE "<5>" /* Normal but significant condition */
#define KERN_INFO "<6>"   /* Informational */
#define KERN_DEBUG "<7>"  /* Debug-level messages */

#define DEFAULT_MESSAGE_LOGLEVEL KERN_INFO

/**
 * @brief Kernel print function.
 *
 * Prints a formatted string to configured kernel message outputs (e.g.,
 * console, serial). Supports a subset of printf format specifiers.
 *
 * @param fmt The format string, similar to printf.
 * Can optionally start with a log level string (e.g., KERN_ERR).
 * @param ... Variable arguments matching the format string.
 * @return The number of characters printed, or a negative value on error.
 */
int printk(const char *fmt, ...);

#endif // SHIT_PRINTK_H