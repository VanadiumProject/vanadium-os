#ifndef KERNEL_H
#define KERNEL_H

#include "types.h"

#define REG8(addr)	(*((volatile ui8_t *)	(addr)))
#define REG16(addr)	(*((volatile ui16_t *)	(addr)))
#define REG32(addr)	(*((volatile ui32_t *)	(addr)))


#endif


