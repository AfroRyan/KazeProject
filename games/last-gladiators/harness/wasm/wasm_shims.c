#include <stddef.h>
void *memset(void *s, int c, size_t n) {
  unsigned char *p = s;
  while (n--) *p++ = (unsigned char)c;
  return s;
}
void *memcpy(void *d, const void *s, size_t n) {
  unsigned char *dd = d;
  const unsigned char *ss = s;
  while (n--) *dd++ = *ss++;
  return d;
}
