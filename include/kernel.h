#ifndef KERNEL_H
#define KERNEL_H

#include "types.h"

// Everything a game is allowed to know about us. Kept separate so the SDK can
// copy one self-contained file instead of a kernel header.
#include "vanadium_abi.h"

#define REG8(addr)	(*((volatile ui8_t *)	(addr)))
#define REG16(addr)	(*((volatile ui16_t *)	(addr)))
#define REG32(addr)	(*((volatile ui32_t *)	(addr)))

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


