#include <kstring.h>

__no_builtin void *memcpy(void *dest, const void *src, size_t n){
    char *d = dest;
    const char *s = src;
    for (; n; n--) *d++ = *s++;
    return dest;
}

__no_builtin void *memmove(void *dest, const void *src, size_t n){
    char *d = dest;
    const char *s = src;
    if (d == s) return d;
    if ((uintptr_t)s - (uintptr_t)d - n <= -2*n) return memcpy(d, s, n);
    if (d < s){
        for (; n; n--) (*d++ = *s++);
    } else {
        while (n) n--, d[n] = s[n];
    }
    return dest;
}

__no_builtin void *memset(void *dest, int c, size_t n){
    char *s = dest;
    for (; n; n--) *s++ = c;
    return dest;
}

__no_builtin int memcmp(const void *vl, const void *vr, size_t n){
    const unsigned char *l = vl, *r = vr;
    for (; n && *l == *r; n--, l++, r++);
    return n ? *l - *r: 0;
}

size_t strlen(const char *str){
    const char *s = str;
    size_t sl = 0;
    while (s[sl]) sl++;
    return sl;
}
