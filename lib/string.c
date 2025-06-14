#include <shit/string.h>

size_t strlen(const char *s) {
  size_t len = 0;
  while (s[len]) {
    len++;
  }
  return len;
}

char *strcpy(char *dest, const char *src) {
  char *orig_dest = dest;
  while ((*dest++ = *src++))
    /* nothing */;

  return orig_dest;
}

char *strncpy(char *dest, const char *src, size_t n) {
  char *orig_dest = dest;
  size_t i;

  for (i = 0; i < n && src[i] != '\0'; i++) {
    dest[i] = src[i];
  }

  for (; i < n; i++) {
    dest[i] = '\0';
  }

  return orig_dest;
}

char *strcat(char *dest, const char *src) {
  char *orig_dest = dest;

  while (*dest) {
    dest++;
  }
  while ((*dest++ = *src++))
    /* nothing */;

  return orig_dest;
}

char *strncat(char *dest, const char *src, size_t n) {
  char *orig_dest = dest;

  while (*dest) {
    dest++;
  }

  size_t i;
  for (i = 0; i < n && src[i] != '\0'; i++) {
    dest[i] = src[i];
  }

  dest[i] = '\0';
  return orig_dest;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  if (n == 0) {
    return 0;
  }
  while (n-- > 0 && *s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }

  if (n == (size_t)-1 || (*s1 == *s2)) {
    return 0;
  }
  return *(const unsigned char *)s1 - *(const unsigned char *)s2;
}

void *memcpy(void *dest_ptr, const void *src_ptr, size_t n) {
  unsigned char *dest = (unsigned char *)dest_ptr;
  const unsigned char *src = (const unsigned char *)src_ptr;
  for (size_t i = 0; i < n; i++) {
    dest[i] = src[i];
  }
  return dest_ptr;
}

void *memset(void *s_ptr, int c, size_t n) {
  unsigned char *s = (unsigned char *)s_ptr;
  unsigned char val = (unsigned char)c;
  for (size_t i = 0; i < n; i++) {
    s[i] = val;
  }
  return s_ptr;
}

int memcmp(const void *s1_ptr, const void *s2_ptr, size_t n) {
  const unsigned char *s1 = s1_ptr;
  const unsigned char *s2 = s2_ptr;
  for (size_t i = 0; i < n; i++) {
    if (s1[i] < s2[i]) {
      return -1;
    }
    if (s1[i] > s2[i]) {
      return 1;
    }
  }
  return 0;
}

void *memmove(void *dest_ptr, const void *src_ptr, size_t n) {
  unsigned char *dest = (unsigned char *)dest_ptr;
  const unsigned char *src = (const unsigned char *)src_ptr;

  if (dest == src || n == 0) {
    return dest_ptr;
  }

  if (dest > src && dest < src + n) {
    for (size_t i = n; i > 0; i--) {
      dest[i - 1] = src[i - 1];
    }
  } else {
    for (size_t i = 0; i < n; i++) {
      dest[i] = src[i];
    }
  }
  return dest_ptr;
}

char *strchr(const char *s, int c) {
  char ch = (char)c;
  while (*s != ch) {
    if (*s == '\0') {
      return NULL;
    }
    s++;
  }
  return (char *)s;
}
