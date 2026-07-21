#ifndef KPRINTF_H
#define KPRINTF_H

// stdarg.h is compiler-provided, like stdint.h. No libc involved.
#include <stdarg.h>

#include "types.h"

// Formatted output over whatever the HAL calls a console.
//
// Supported: %d %i %u %x %X %p %c %s %%
// Flags:     '0' for zero padding, then a decimal field width
// Length:    'l', 'll' and 'z' all mean 64 bit, which is what LP64 gives us.
//            On this target an int is 32 bit and a pointer is 64, so register
//            dumps want %lx and forgetting the l truncates them silently.
void kprintf(const char *fmt, ...);
void kvprintf(const char *fmt, va_list ap);

#endif
