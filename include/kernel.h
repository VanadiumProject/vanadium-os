#ifndef KERNEL_H
#define KERNEL_H

#include "types.h"

#define REG8(addr)	(*((volatile ui8_t *)	(addr)))
#define REG16(addr)	(*((volatile ui16_t *)	(addr)))
#define REG32(addr)	(*((volatile ui32_t *)	(addr)))

// SYSCALL MACROS

#define SYSCALL_WRITE      0x00
#define SYSCALL_READ       0x01
#define SYSCALL_EXIT       0x03
#define SYSCALL_GET_INPUT  0x10
#define SYSCALL_PGPU_CMD   0x20
#define SYSCALL_SBRK       0x30

// RESERVED, deliberately not implemented. These become real syscalls the day
// there is a filesystem to answer them. Until then the SDK answers them
// locally, because with a fixed set of descriptors the answers are known at
// compile time and a trap buys nothing.
//
// fstat is the clearest case: implementing it here would mean pinning down a
// struct stat layout now, with no filesystem to inform what belongs in it.
// That is the wrong order to decide an ABI in.
//
// isatty gets no number at all. It is not a syscall even on Unix; a libc
// derives it from fstat.
#define SYSCALL_CLOSE      0x04
#define SYSCALL_FSTAT      0x05
#define SYSCALL_LSEEK      0x06

// THE CONTRACT WITH A GAME
//
// These three descriptors are valid from the game's first instruction. There
// is no open(): that would need a filesystem, and a console does not need one
// to print. Any other descriptor is an error.
//
// Where they physically go is ours to change. Today all output lands on the
// debug UART because it can be read without a shell, a screen or a driver.
// When there is a video console, or a Safety Modes log, fd 1 and fd 2 can be
// pointed at them without recompiling a single game. The number is the
// contract; the destination is not.
#define FD_STDIN           0
#define FD_STDOUT          1
#define FD_STDERR          2

// Syscall handler called from assembly trap
// a7: syscall number, a0-a3: arguments
// Returns the value the trap handler writes back into the caller's a0.
uintptr_t syscall_handler(uintptr_t syscall_num, uintptr_t arg0, uintptr_t arg1, uintptr_t arg2);

// Called from the trap handler for anything that is not a syscall.
// Reports the cause over UART and halts; never returns.
void trap_fatal_handler(uintptr_t scause, uintptr_t sepc, uintptr_t stval);

// Handed to us by the previous boot stage and stashed in boot.S.
// boot_dtb is the device tree pointer; nothing parses it yet.
extern uintptr_t boot_hartid;
extern uintptr_t boot_dtb;

#endif


