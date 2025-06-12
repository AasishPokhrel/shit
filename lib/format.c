#include <shit/format.h>
#include <stdarg.h>
#include <stddef.h>

char *itoa_k(long long value, char *str, int base, int is_signed, int min_width,
             char pad_char) {
  char *ptr = str, *ptr1 = str, tmp_char;
  long long tmp_value;
  int len = 0;
  unsigned long long u_value = (unsigned long long)value;

  if (base < 2 || base > 36) {
    if (str)
      *str = '\0';
    return str;
  }

  if (is_signed && value < 0) {
    u_value = (unsigned long long)-value;
  }

  do {
    tmp_value = u_value;
    u_value /= base;
    *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[tmp_value % base];
    len++;
  } while (u_value);

  while (len < min_width) {
    *ptr++ = pad_char;
    len++;
  }

  if (is_signed && value < 0) {
    *ptr++ = '-';
  }

  *ptr-- = '\0';

  while (ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr-- = *ptr1;
    *ptr1++ = tmp_char;
  }

  return str;
}

static void append_char(char **buf_ptr, char c, size_t *remaining_size,
                        int *written_count) {
  if (*remaining_size > 1) { // Leave space for null terminator
    **buf_ptr = c;
    (*buf_ptr)++;
    (*remaining_size)--;
  }
  (*written_count)++;
}

// Helper for kvsnprintf to append a string to the buffer
static void append_string(char **buf_ptr, const char *s, size_t *remaining_size,
                          int *written_count) {
  if (!s)
    s = "(null)";
  while (*s && *remaining_size > 1) {
    **buf_ptr = *s;
    (*buf_ptr)++;
    s++;
    (*remaining_size)--;
    (*written_count)++;
  }

  while (*s) {
    s++;
    (*written_count)++;
  }
}

int kvsnprintf(char *buf, size_t size, const char *fmt, va_list args) {
  if (!buf || !fmt || size == 0)
    return -1;

  int written_count = 0;
  size_t remaining_size = size;

  while (*fmt) {
    if (*fmt != '%') {
      append_char(&buf, *fmt, &remaining_size, &written_count);
      fmt++;
      continue;
    }

    fmt++;
    char pad_char = ' ';
    int min_width = 0;
    // int precision = -1; // Not implementing precision for now
    int is_long = 0;
    int is_long_long = 0;

    if (*fmt == '0') {
      pad_char = '0';
      fmt++;
    }

    while (*fmt >= '0' && *fmt <= '9') {
      min_width = min_width * 10 + (*fmt - '0');
      fmt++;
    }

    if (*fmt == 'l') {
      is_long = 1;
      fmt++;
      if (*fmt == 'l') {
        is_long_long = 1;
        fmt++;
      }
    }

    char temp_num_buf[22]; // For 64-bit numbers in binary + sign + null

    switch (*fmt) {
    case 'c': {
      char c_val = (char)va_arg(args, int);
      append_char(&buf, c_val, &remaining_size, &written_count);
      break;
    }
    case 's': {
      const char *s_val = va_arg(args, const char *);
      append_string(&buf, s_val, &remaining_size, &written_count);
      break;
    }
    case 'd':
    case 'i': {
      long long val;
      if (is_long_long)
        val = va_arg(args, long long);
      else if (is_long)
        val = va_arg(args, long);
      else
        val = va_arg(args, int);
      itoa_k(val, temp_num_buf, 10, 1, min_width, pad_char);
      append_string(&buf, temp_num_buf, &remaining_size, &written_count);
      break;
    }
    case 'u': {
      unsigned long long val;
      if (is_long_long)
        val = va_arg(args, unsigned long long);
      else if (is_long)
        val = va_arg(args, unsigned long);
      else
        val = va_arg(args, unsigned int);
      itoa_k(val, temp_num_buf, 10, 0, min_width, pad_char);
      append_string(&buf, temp_num_buf, &remaining_size, &written_count);
      break;
    }
    case 'x':
    case 'X': {
      unsigned long long val;
      if (is_long_long)
        val = va_arg(args, unsigned long long);
      else if (is_long)
        val = va_arg(args, unsigned long);
      else
        val = va_arg(args, unsigned int);
      itoa_k(val, temp_num_buf, 16, 0, min_width, pad_char);
      append_string(&buf, temp_num_buf, &remaining_size, &written_count);
      break;
    }
    case 'p': {
      uintptr_t ptr_val = (uintptr_t)va_arg(args, void *);
      if (remaining_size > 3) {
        append_char(&buf, '0', &remaining_size, &written_count);
        append_char(&buf, 'x', &remaining_size, &written_count);
      } else {
        written_count += 2;
      }

      int ptr_min_width = (sizeof(void *) == 8) ? 16 : 8;
      if (min_width > 0 && min_width > ptr_min_width)
        ptr_min_width = min_width;

      itoa_k(ptr_val, temp_num_buf, 16, 0, ptr_min_width,
             '0'); // Pad pointers with '0'
      append_string(&buf, temp_num_buf, &remaining_size, &written_count);
      break;
    }
    case '%': {
      append_char(&buf, '%', &remaining_size, &written_count);
      break;
    }
    default:
      if (remaining_size > 1) {
        *buf++ = '%';
        remaining_size--;
        written_count++;
      }
      if (*fmt && remaining_size > 1) {
        *buf++ = *fmt;
        remaining_size--;
        written_count++;
      } else if (!*fmt) {
        goto ensure_null_term;
      }
      break;
    }
    fmt++;
  }

ensure_null_term:
  if (size > 0) {
    *buf = '\0';
  }
  // If buffer was too small from the start to even hold null terminator
  // and size was 1, buf_start[0] was written by append_char if any char was
  // processed. If size was 0, kvsnprintf returned -1. If size was 1, and
  // something was written, buf_start[0] has that char, it should be '\0'. The
  // append_char/string already ensure remaining_size > 1 for writing. So if
  // remaining_size becomes 1, *buf is where '\0' goes.

  return written_count;
}