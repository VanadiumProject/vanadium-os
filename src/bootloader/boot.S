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
	la sp, _stack_top	# Load address of stack top
	la t0, trap_handler
	csrw mtvec, t0		# Set Trap Vector address
	call kmain		# Call C kernel Main

park:
	wfi
	j park

.align 4
trap_handler:
	# Save caller-saved registers
	addi sp, sp, -64
	sw ra, 0(sp)
	sw t0, 4(sp)
	sw t1, 8(sp)
	sw t2, 12(sp)
	sw a0, 16(sp)
	sw a1, 20(sp)
	sw a2, 24(sp)
	sw a3, 28(sp)
	sw a4, 32(sp)
	sw a5, 36(sp)
	sw a6, 40(sp)
	sw a7, 44(sp)
	sw t3, 48(sp)
	sw t4, 52(sp)
	sw t5, 56(sp)
	sw t6, 60(sp)

	# Call C handler: syscall_handler(a7, a0, a1, a2)
	mv a0, a7
	lw a1, 16(sp)	# Original a0
	lw a2, 20(sp)	# Original a1
	lw a3, 24(sp)	# Original a2
	call syscall_handler

	# Move mepc forward by 4 (skip the ecall instruction)
	csrr t0, mepc
	addi t0, t0, 4
	csrw mepc, t0

	# Restore registers
	lw ra, 0(sp)
	lw t0, 4(sp)
	lw t1, 8(sp)
	lw t2, 12(sp)
	lw a0, 16(sp)
	lw a1, 20(sp)
	lw a2, 24(sp)
	lw a3, 28(sp)
	lw a4, 32(sp)
	lw a5, 36(sp)
	lw a6, 40(sp)
	lw a7, 44(sp)
	lw t3, 48(sp)
	lw t4, 52(sp)
	lw t5, 56(sp)
	lw t6, 60(sp)
	addi sp, sp, 64
	mret
		
