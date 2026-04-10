#include "kernel.h"
#include "hardware.h"
#include "hal.h"


void kmain(void) {
	const uart_hal_t *hal = get_hal();

	hal->uart_puts("Yes, I am alive!\n");
	while (1);
}
