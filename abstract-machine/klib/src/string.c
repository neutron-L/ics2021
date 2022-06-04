#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  // panic("Not implemented");
  size_t len = 0;
  while (*s != '\0')
  {
    len++;
    s++;
  }

  return len;
}

char *strcpy(char *dst, const char *src) {
  // panic("Not implemented");
  char * p = dst;
  while ((*dst = *src) != '\0')
  {
    dst++;
    src++;
  }

  return p;
}

char *strncpy(char *dst, const char *src, size_t n) {
  // panic("Not implemented");
  size_t i;

  for (i = 0; i < n && src[i] != '\0'; i++)
      dst[i] = src[i];
  for ( ; i < n; i++)
      dst[i] = '\0';

  return dst;

}

char *strcat(char *dst, const char *src) {
  // panic("Not implemented");
  size_t len = strlen(dst);
  char *p = dst + len;

  while ((*p = *src) != '\0')
  {
    p++;
    src++;
  }

  return dst;
}

int strcmp(const char *s1, const char *s2) {
  // panic("Not implemented");
  while (*s1 == *s2 && *s1 != '\0')
  {
    s1++;
    s2++;
  }
  if (*s1 == *s2)
    return 0;
  else if (*s1 < *s2)
    return -1;
  return 1;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  // panic("Not implemented");
  if (n <= 0)
    return 0;
  while (*s1 == *s2 && *s1 != '\0' && n--)
  {
    s1++;
    s2++;
  }
  if (*s1 == *s2)
    return 0;
  else if (*s1 < *s2)
    return -1;
  return 1;
}

void *memset(void *s, int c, size_t n) {
  // panic("Not implemented");
  char * p = (char *)s;
  while (n--)
  {
    *p = (char)c;
    p++;
  }
  *p = '\0';

  return s;
}

void *memmove(void *dst, const void *src, size_t n) {
  // panic("Not implemented");
  if (dst <= src)
  {
    char * dp = (char *)dst;
    const char * sp = (char *)src;
    while (n--)
    {
      *dp++ = *sp++;
    }
  }
  else
  {
    char * dp = (char *)dst + n;
    const char * sp = (char *)src + n;
    while (n--)
    {
      *dp-- = *sp--;
    }
  }
  return dst;
}

void *memcpy(void *out, const void *in, size_t n) {
  // panic("Not implemented");
  char * dp = (char *)out;
  const char * sp = (char *)in;
  while (n--)
  {
    *dp++ = *sp++;
  }
  return out;
}

int memcmp(const void *s1, const void *s2, size_t n) {
  // panic("Not implemented");
  char * dp = (char *)s1;
  const char * sp = (char *)s2;
  return strncmp(dp, sp, n);
}

#endif
