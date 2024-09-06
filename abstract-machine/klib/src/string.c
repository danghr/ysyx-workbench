#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  char *p = (char *)s;
  size_t cnt = 0;
  // Exclude the null terminator
  while (*(p++) != '\0')
    cnt++;
  return cnt;
}

char *strcpy(char *dst, const char *src) {
  // Let's use `strncpy` to implement strcpy
  // Note that `strncpy` automatically stops at the null terminator
  // so we can safely set the length to maximum integer
  return strncpy(dst, src, INT32_MAX);
}

char *strncpy(char *dst, const char *src, size_t n) {
  // Follow the implementation of `strncpy` in man 3
  // Note that AT MOST `n` bytes are copied
  // Stop at the first null terminator if `n` is larger than the length of `src`
  // Note to fill the null terminator in this case
  size_t i;
  for (i = 0; i < n && src[i] != '\0'; i++)
    dst[i] = src[i];
  if (src[i] != '\0')
    for (; i < n; i++)
      dst[i] = '\0';
  return dst;
}

char *strcat(char *dst, const char *src) {
  // Let's use `strncat` to implement strcat
  // Note that `strncat` automatically stops at the null terminator
  // so we can safely set the length to maximum integer
  return strncat(dst, src, INT32_MAX);
}

// Add a function here
char *strncat(char *dst, const char *src, size_t n) {
  // Follow the implementation of `strncat` in man 3
  // Note that AT MOST `n` bytes are copied
  // Stop at the first null terminator if `n` is larger than the length of `src`
  // Note to fill the null terminator in this case
  size_t dst_len = strlen(dst);
  size_t i;
  for (i = 0; i < n && src[i] != '\0'; i++)
    dst[dst_len + i] = src[i];
  // Note that the null terminator is never copied up to now
  // as `strlen` does not count it
  dst[dst_len + i] = '\0';

  return dst;
}

int strcmp(const char *s1, const char *s2) {
  return strncmp(s1, s2, INT32_MAX);
}

int strncmp(const char *s1, const char *s2, size_t n) {
  // Compare the two strings character by character
  // Stop at the first difference or the end of either string
  // Return the difference of the first different characters
  // i.e., s1[i] - s2[i]
  // Note that this is not the definition of `strncmp`, but the common implementation.
  for (size_t i = 0; i < n; i++) {
    // First difference
    // It also handles the case when either string ends or when `n` is reached
    if ((s1[i] != s2[i]) || i == n - 1)
      return (int)s1[i] - (int)s2[i]; // Avoid compiler from using unsigned char
    // When both string ends and no difference is found
    if (s1[i] == '\0')
      return 0;
  }
  // This line should never be reached
  assert(0);
  return 0;
}

void *memset(void *s, int c, size_t n) {
  // Maybe we should treat `c` as `unsigned char`?
  for (size_t i = 0; i < n; i++)
    ((char *)s)[i] = (unsigned char)c;
  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  // Note that this handles the case when `src` and `dst` overlap
  // According to man 3, we need to create a buffer to store the content of `src`
  // and then copy the content to `dst`
  void *buffer = malloc(n);
  memcpy(buffer, src, n);
  memcpy(dst, buffer, n);
  free(buffer);
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  for (size_t i = 0; i < n; i++)
    ((char *)in)[i] = ((char *)out)[i];
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  return strncmp((char *)s1, (char *)s2, n);
}

#endif
