#include "kernel.h"
#include "hal.h"

static ui32_t current_input = 0;

static void kputhex(uintptr_t v) {
	const uart_hal_t *hal = get_hal();
	const char *digits = "0123456789abcdef";

	hal->uart_puts("0x");
	for (i32_t i = 60; i >= 0; i -= 4)
		hal->uart_putc(digits[(v >> i) & 0xf]);
}

// Placeholder reporting until kprintf exists. Anything that is not a syscall
// lands here instead of silently resuming at the wrong instruction.
void trap_fatal_handler(uintptr_t mcause, uintptr_t mepc, uintptr_t mtval) {
	const uart_hal_t *hal = get_hal();

	hal->uart_puts("\n!! FATAL TRAP !!\n  mcause = ");
	kputhex(mcause);
	hal->uart_puts("\n  mepc   = ");
	kputhex(mepc);
	hal->uart_puts("\n  mtval  = ");
	kputhex(mtval);
	hal->uart_puts("\nHALTED\n");

	while (1) __asm__ volatile ("wfi");
}

uintptr_t syscall_handler(uintptr_t syscall_num, uintptr_t arg0, uintptr_t arg1, uintptr_t arg2) {
	(void)arg1; (void)arg2; // Silence unused warnings
	const uart_hal_t *hal = get_hal();

	switch (syscall_num) {
		case SYSCALL_WRITE:
			hal->uart_putc((char)arg0);
			return 0;
		case SYSCALL_GET_INPUT:
			// The trap handler now writes this into the caller's a0 slot
			// before restoring, so the value actually reaches the caller.
			// In the final version this reads GPIO.
			return current_input;
		default:
			return (uintptr_t)-1;
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
