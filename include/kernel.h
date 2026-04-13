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

// Syscall handler called from assembly trap
// a7: syscall number, a0-a3: arguments
void syscall_handler(ui32_t syscall_num, ui32_t arg0, ui32_t arg1, ui32_t arg2);

#endif


