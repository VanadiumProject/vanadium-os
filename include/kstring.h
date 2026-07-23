#ifndef KSTRING_H
#define KSTRING_H

#include <stddef.h>

#define __no_builtin __attribute__((no_builtin))

void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *dest, int c, size_t n);
int memcmp(const void *vl, const void *vr, size_t n);
size_t strlen(const char *str);


#endif
