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

// Laid out by the linker script, not by us. See src/linker.ld
extern ui8_t _user_heap_start[];
extern ui8_t _user_heap_end[];

static ui8_t *user_brk = 0;

// sbrk is the hook picolibc and newlib expect malloc to sit on: move the break
// by an increment, hand back where it used to be. With no MMU there is nothing
// to map, so this is a bump pointer inside a region the linker already fixed.
static uintptr_t sys_sbrk(intptr_t increment) {
	ui8_t *prev;
	ui8_t *next;

	if (!user_brk)
		user_brk = _user_heap_start;

	prev = user_brk;
	next = user_brk + increment;

	if (next < _user_heap_start || next > _user_heap_end)
		return (uintptr_t)-1;

	user_brk = next;
	return (uintptr_t)prev;
}

uintptr_t syscall_handler(uintptr_t syscall_num, uintptr_t arg0, uintptr_t arg1, uintptr_t arg2) {
	const uart_hal_t *hal = get_hal();

	switch (syscall_num) {
		case SYSCALL_WRITE: {
			// (fd, buffer, length). The pointer comes from U-mode and we
			// cannot check it: there is no hardware between us and the
			// game, by decision. Once the memory map fixes where a game
			// lives, a range check belongs right here.
			const char *buf = (const char *)arg1;
			size_t len = (size_t)arg2;

			if (arg0 != FD_STDOUT && arg0 != FD_STDERR)
				return (uintptr_t)-1;

			for (size_t i = 0; i < len; i++)
				hal->uart_putc(buf[i]);

			return len;
		}

		case SYSCALL_READ: {
			// Blocking, and stops at a newline the way a line buffered
			// console is expected to. This exists so that fd 0 is not a
			// promise we fail to keep; a game is not meant to live here.
			char *buf = (char *)arg1;
			size_t len = (size_t)arg2;
			size_t i = 0;

			if (arg0 != FD_STDIN)
				return (uintptr_t)-1;

			while (i < len) {
				char c = (char)hal->uart_getc();

				buf[i++] = c;
				if (c == '\n' || c == '\r')
					break;
			}
			return i;
		}

		case SYSCALL_EXIT:
			// Never returns. If it did, the trap handler would sret back
			// into a game that has already finished. There is nowhere to
			// return to yet either: one game is the whole system.
			kprintf("\n[%lu ms] Game exited, code %ld\n", ktime_ms(), (i64_t)arg0);
			kprintf("HALTED\n");
			while (1)
				__asm__ volatile ("wfi");

		case SYSCALL_SBRK:
			return sys_sbrk((intptr_t)arg0);

		case SYSCALL_GET_INPUT:
			// The trap handler now writes this into the caller's a0 slot
			// before restoring, so the value actually reaches the caller.
			// In the final version this reads GPIO.
			(void)arg1; (void)arg2;
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
