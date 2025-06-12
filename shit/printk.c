#include <shit/console.h>
#include <shit/format.h>
#include <shit/printk.h>
#include <stdarg.h>

#define PRINTK_BUFFER_SIZE 256
#define KERNEL_LOG_BUFFER_SIZE (4 * 1024)

static char temp_printk_buffer[PRINTK_BUFFER_SIZE];

int printk(const char *fmt, ...) {
  va_list args;
  int count;

  va_start(args, fmt);
  count = kvsnprintf(temp_printk_buffer, PRINTK_BUFFER_SIZE, fmt, args);
  va_end(args);

  if (count > 0) {
    console_writestring(temp_printk_buffer);
  }

  return count;
}