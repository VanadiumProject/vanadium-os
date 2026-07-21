#ifndef TYPES_H
#define TYPES_H

// stdint.h/stddef.h are provided by the compiler, not by a libc, so they are
// safe in a freestanding kernel. size_t, uintptr_t and intptr_t come from
// there: defining them by hand fights the ABI's own definitions and breaks
// the moment anything includes a compiler header (stdarg.h for kprintf...).
#include <stdint.h>
#include <stddef.h>

typedef uint8_t		ui8_t;
typedef uint16_t	ui16_t;
typedef uint32_t	ui32_t;
typedef uint64_t	ui64_t;

typedef int8_t		i8_t;
typedef int16_t		i16_t;
typedef int32_t		i32_t;
typedef int64_t		i64_t;

typedef struct {
	uintptr_t xregs[32];
	uintptr_t mepc;
	uintptr_t mstatus;
	uintptr_t mcause;
	uintptr_t mtval;
	uintptr_t kernel_sp;
} trap_frame_t;

// main_trap_frame in src/bootloader/boot.S reserves this many bytes by hand.
// If this fires, the two went out of sync.
_Static_assert(sizeof(trap_frame_t) == 296, "trap_frame_t must match .space in boot.S");

#endif
