#include "kernel.h"
#include "hal.h"

#include "kernel.h"
#include "hardware.h"
#include "hal.h"

static ui32_t current_input = 0;

void syscall_handler(ui32_t syscall_num, ui32_t arg0, ui32_t arg1, ui32_t arg2) {
	(void)arg0; (void)arg1; (void)arg2; // Silence unused warnings
	const uart_hal_t *hal = get_hal();

	switch (syscall_num) {
		case SYSCALL_WRITE:
			hal->uart_putc((char)arg0);
			break;
		case SYSCALL_GET_INPUT:
			// In final version, this reads GPIO. For now, it returns current_input
			// which we'll update in kmain for testing.
			// Actually, let's return it in a0 (which is the caller's a0 when we restore)
			// Wait, the trap handler needs to know where to put the return value.
			// Let's modify trap_handler to put syscall_handler's return value into a0.
			break;
	}
}

void kmain(void) {
	const uart_hal_t *hal = get_hal();
	hal->uart_puts("Vanadium Kernel Initialized\n");
	hal->uart_puts("WASD = D-Pad, IJKL = Buttons\n");

	while (1) {
		ui8_t c = hal->uart_getc();
		// Quick mapping for testing your WASD/IJKL idea
		switch(c) {
			case 'w': current_input |= (1 << 0); break;
			case 's': current_input |= (1 << 1); break;
			case 'a': current_input |= (1 << 2); break;
			case 'd': current_input |= (1 << 3); break;
			case 'l': current_input |= (1 << 4); break;
			case 'k': current_input |= (1 << 5); break;
			case 'i': current_input |= (1 << 6); break;
			case 'j': current_input |= (1 << 7); break;
			case ' ': current_input = 0; hal->uart_puts("Reset Input\n"); break;
		}

		if (current_input) {
			hal->uart_puts("Input: ");
			hal->uart_putc(c);
			hal->uart_puts("\n");
		}
	}
}
