#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  // In order not to disrupt the execution of `assertion`
  // we directly return 0 for this function
  return 0;
  // panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  // Read the format string
  const int ERROR_RETURN = -1;
  int out_count = 0;
  for (const char *p = fmt; *p != '\0'; p++) {
    // Handle normal characters
    if (*p != '%') {
      out[out_count++] = *p;
      continue;
    }
    // Handle format specifiers
    // Now we know that *p == '%'
    if (*(p + 1) == 's') {
      const char *s = va_arg(ap, const char *);
      while (*s != '\0') {
        out[out_count++] = *(s++);
      }
      p++;
    } else if (*(p + 1) == 'd') {
      int n = va_arg(ap, int);
      if (n == -2147483648) {
        // Specially handle this case
        // as the following method cannot convert -2147483648 to a positive number
        out[out_count++] = '-';
        out[out_count++] = '2';
        out[out_count++] = '1';
        out[out_count++] = '4';
        out[out_count++] = '7';
        out[out_count++] = '4';
        out[out_count++] = '8';
        out[out_count++] = '3';
        out[out_count++] = '6';
        out[out_count++] = '4';
        out[out_count++] = '8';
        p++;
        continue;
      }
      // We cannot use `stoi` here as we are implementing a library function
      // Output a negative symbol
      if (n < 0) {
        out[out_count++] = '-';
        n = -n;
      }
      // Use a buffer to store the digits
      // Max: 2147483647, 10 digits
      char buf[11];
      buf[10] = '\0';
      int loc = 9;
      do {
        buf[loc--] = n % 10 + '0';
        n /= 10;
      } while (n > 0);
      // Output the digits
      for (int i = loc + 1; i < 10; i++) {
        out[out_count++] = buf[i];
      }
      p++;
    } else if (*(p + 1) == 'u') {
      unsigned int n = va_arg(ap, unsigned int);
      // We cannot use `stoi` here as we are implementing a library function
      // Use a buffer to store the digits
      // Max: 4294967295, 10 digits
      char buf[11];
      buf[10] = '\0';
      int loc = 9;
      do {
        buf[loc--] = n % 10 + '0';
        n /= 10;
      } while (n > 0);
      // Output the digits
      for (int i = loc + 1; i < 10; i++) {
        out[out_count++] = buf[i];
      }
      p++;
    } else {
      return ERROR_RETURN;
    }
  }
  out[out_count] = '\0';
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
