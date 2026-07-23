#ifndef VANADIUM_ABI_H
#define VANADIUM_ABI_H

// THE CONTRACT BETWEEN VANADIUMOS AND A GAME.
//
// This header is the whole boundary. The OS repository owns it because the OS
// implements it; the SDK carries a copy and records which commit it came from.
// Nothing kernel-internal may ever be added here: if the SDK does not need it,
// it does not belong.
//
// It deliberately depends on nothing, not even our own types.h, so that the
// SDK can include it before it has a C library working.

// Bumped whenever a number or a signature changes. The SDK should refuse to
// build against an ABI it was not written for, because a silent mismatch here
// shows up as a game misbehaving rather than as a compile error.
#define VANADIUM_ABI_VERSION	1

// HOW A SYSCALL IS MADE
//
// Syscall number in a7, arguments in a0-a2, then ecall. The return value comes
// back in a0. An unknown number returns (uintptr_t)-1.
//
// This works from U-mode only. An ecall made from S-mode does not reach the
// kernel at all: it is not delegatable and traps to the firmware as an SBI
// call instead.

#define SYSCALL_WRITE      0x00	// (fd, buf, len)   -> bytes written
#define SYSCALL_READ       0x01	// (fd, buf, len)   -> bytes read
#define SYSCALL_EXIT       0x03	// (code)           -> does not return
#define SYSCALL_CLOSE      0x04	// RESERVED, needs a filesystem
#define SYSCALL_FSTAT      0x05	// RESERVED, needs a filesystem
#define SYSCALL_LSEEK      0x06	// RESERVED, needs a filesystem
#define SYSCALL_GET_INPUT  0x10	// ()               -> button bitmap
#define SYSCALL_PGPU_CMD   0x20	// RESERVED, no GPU behind it yet
#define SYSCALL_SBRK       0x30	// (increment)      -> previous break

// The reserved three are answered by the SDK today, not by a trap: with a
// fixed descriptor set the answers are known at compile time. isatty gets no
// number because it is not a syscall even on Unix; a libc derives it from
// fstat.

// THE DESCRIPTORS
//
// These are valid from a game's first instruction. There is no open(): that
// would need a filesystem, and a console does not need one to print. Any other
// descriptor is an error.
//
// Where they physically go is ours to change. Today everything lands on the
// debug UART because it can be read without a shell, a screen or a driver.
// When there is a video console, or a Safety Modes log, fd 1 and fd 2 can be
// pointed at them without recompiling a single game. The number is the
// contract; the destination is not.
#define FD_STDIN           0
#define FD_STDOUT          1
#define FD_STDERR          2

// HOW A GAME IS ENTERED
//
// U-mode, entered with sret at the binary's entry point.
//   sp        already points at the top of the game's stack
//   a0, a1    zero, reserved, do not read meaning into them
//   argc/argv none, a console has no command line
// The three descriptors above are already valid at that point.

#endif
