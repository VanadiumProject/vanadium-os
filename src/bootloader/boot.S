#include "hardware.h"

.section .text.init
.global _start

_start:
	csrr t0, mhartid	# Read Machine Hardware thread ID to t0
	bnez t0, park		# If t0 is not zero, branch(jmp) to park

	la t0, _bss_start
	la t1, _bss_end
bss_loop:
	bgeu t0, t1, bss_done
	sw zero, 0(t0)
	addi t0, t0, 4
	j bss_loop

bss_done:
	la sp, _stack_top	# Load address of stack top (defined on linker script)
	call kmain		# Call C kernel Main

park:
	wfi	# Waits for interrupt (puts core to sleep)
	j park	# Loops back to infinity if woken up
		
