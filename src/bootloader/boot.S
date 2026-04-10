#include "hardware.h"

.section .text.init
.global _start

_start:
	csrr t0, mhartid	# Read Machine Hardware thread ID to t0
	bnez t0, park		# If t0 is not zero, branch(jmp) to park

	la sp, _stack_top	# Load address of stack top (defined on linker script)

	call kmain		# Call C kernel Main

park:
	wfi	# Waits for interrupt (puts core to sleep)
	j park	# Loops back to infinity if woken up
		
