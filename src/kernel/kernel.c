#include "kernel.h"
#include "kprintf.h"
#include "ktime.h"
#include "hal.h"

static ui32_t current_input = 0;

// Names come straight from the privileged spec's exception code table. An
// error code is useless if it doesn't tell you where to look.
static const char *trap_cause_name(uintptr_t scause) {
	if ((i64_t)scause < 0)
		return "Interrupt";

	switch (scause) {
		case 0:  return "Instruction address misaligned";
		case 1:  return "Instruction access fault";
		case 2:  return "Illegal instruction";
		case 3:  return "Breakpoint";
		case 4:  return "Load address misaligned";
		case 5:  return "Load access fault";
		case 6:  return "Store/AMO address misaligned";
		case 7:  return "Store/AMO access fault";
		case 8:  return "Environment call from U-mode";
		case 12: return "Instruction page fault";
		case 13: return "Load page fault";
		case 15: return "Store/AMO page fault";
		default: return "Unknown cause";
	}
}

// Anything that is not a syscall lands here instead of silently resuming at
// the wrong instruction.
void trap_fatal_handler(uintptr_t scause, uintptr_t sepc, uintptr_t stval) {
	// Milliseconds since boot, per the log convention in Safety_Modes.
	kprintf("\n[%lu ms] !! FATAL TRAP !! %s\n", ktime_ms(), trap_cause_name(scause));
	kprintf("  scause = 0x%016lx\n", scause);
	kprintf("  sepc   = 0x%016lx\n", sepc);
	kprintf("  stval  = 0x%016lx\n", stval);
	kprintf("HALTED\n");

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
