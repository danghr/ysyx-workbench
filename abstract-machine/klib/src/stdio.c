#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

void putchar_printf(char ch, char *out, int *out_count) {
  out[(*out_count)++] = ch;
  putstr("Printing character ");
  putch(ch);
  putch('\n');
}

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  // Read the format string
  // const int ERROR_RETURN = -1;
  int out_count = 0;
  // for (const char *p = fmt; *p != '\0'; p++) {
  //   // Handle normal characters
  //   if (*p != '%') {
  //     putchar_printf(*p, out, &out_count);
  //     continue;
  //   }
  //   // Handle format specifiers
  //   // Now we know that *p == '%'
  //   if (*(p + 1) == 's') {
  //     const char *s = va_arg(ap, const char *);
  //     while (*s != '\0') {
  //       putchar_printf(*(s++), out, &out_count);
  //     }
  //   } else if (*(p + 1) == 'd') {
  //     int n = va_arg(ap, int);
  //     // We cannot use `stoi` here as we are implementing a library function
  //     // Output a negative symbol
  //     if (n < 0) {
  //       putchar_printf('-', out, &out_count);
  //       n = -n;
  //     }
  //     // Use a buffer to store the digits
  //     // Max: 2147483647, 10 digits
  //     char buf[11];
  //     buf[10] = '\0';
  //     int loc = 9;
  //     while (n > 0) {
  //       buf[loc--] = n % 10 + '0';
  //       n /= 10;
  //     }
  //     // Output the digits
  //     for (int i = loc + 1; i < 10; i++) {
  //       putchar_printf(buf[i], out, &out_count);
  //     }
  //   } else if (*(p + 1) == '\0') {
  //     putstr("Invalid format specifier \%\n");
  //     return ERROR_RETURN;
  //   } else {
  //     putstr("Invalid format specifier \%");
  //     putch(*(p + 1));
  //     putch('\n');
  //     return ERROR_RETURN;
  //   }
  // }
  // out[out_count] = '\0';
  return out_count;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
