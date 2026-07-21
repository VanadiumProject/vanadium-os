#ifndef TYPES_H
#define TYPES_H

	
typedef unsigned char		ui8_t;
typedef unsigned short		ui16_t;
typedef unsigned int		ui32_t;
typedef unsigned long long	ui64_t;

typedef signed char		i8_t;
typedef signed short		i16_t;
typedef signed int		i32_t;
typedef signed long long	i64_t;

typedef unsigned long long	size_t;
typedef unsigned long long	uintptr_t;
typedef signed long long	intptr_t;

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
