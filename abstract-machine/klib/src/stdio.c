#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  // panic("Not implemented");
  int cnt = 0;
  va_list ap;
  va_start(ap, fmt);
  // args
  char * s;
  int num;
  char ch;
  char cs[32]; // for store each digit of the num
  int idx;  // the index of cs

  while (*fmt != '\0')
  {
    if (*fmt != '%')
    {
      putch(*fmt++);
      cnt++;
    }
    else
    {
      fmt++;
      switch(*fmt) 
      {
        case 's':
        {
          s = va_arg(ap, char *);
          while (*s != '\0')
          {
            putch(*s++);
            cnt++;
          }
        }
        break;
        case 'd':
        {
          num = va_arg(ap, int);
          idx = 0;
          do
          {
            cs[idx++] = '0' + num % 10;
            num /= 10;
          } while (num);
          
          idx--;
          while (idx >= 0)
          {
            putch(cs[idx--]);
            cnt++;
          }
        }
        break;
        case 'c':
        {
          ch = (char)va_arg(ap, int);
          putch(ch);
          cnt++;
        }
        break;
        default:
        return -1;
      }
      fmt++;
    }
  }

  va_end(ap);
  return cnt;

}

int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  // panic("Not implemented");
  char * origin = out;
  va_list ap;
  va_start(ap, fmt);
  char * s;
  int num;
  char cs[32]; // for store each digit of the num
  int idx;  // the index of cs

  while (*fmt != '\0')
  {
    if (*fmt != '%')
      *out++ = *fmt++;
    else
    {
      fmt++;
      switch(*fmt) 
      {
        case 's':
        {
          s = va_arg(ap, char *);
          while (*s != '\0')
            *out++ = *s++;
        }
        break;
        case 'd':
        {
          num = va_arg(ap, int);
          idx = 0;
          do
          {
            cs[idx++] = '0' + num % 10;
            num /= 10;
          } while (num);
          
          idx--;
          while (idx >= 0)
            *out++ = cs[idx--];
        }
        break;
        default:
        panic("sprintf format error\n");
        return -1;
      }
      fmt++;
    }
  }

  va_end(ap);
  *out = '\0';
  return out - origin;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
