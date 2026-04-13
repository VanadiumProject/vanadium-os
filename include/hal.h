#ifndef HAL_H
#define HAL_H

#include "types.h"

// For now, the hal is mostly dedicated to the functions that will need to be
// defined for both cases, virt and the real hardware, but have different
// values/implementations

typedef struct{
	void (*uart_putc)(char c);
	void (*uart_puts)(const char* c);
	ui8_t (*uart_getc)(void);
}uart_hal_t;

const uart_hal_t* get_hal(void);

#endif

