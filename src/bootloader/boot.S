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
	sd zero, 0(t0)
	addi t0, t0, 8
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
	addi sp, sp, -128
	sd ra, 0(sp)
	sd t0, 8(sp)
	sd t1, 16(sp)
	sd t2, 24(sp)
	sd a0, 32(sp)
	sd a1, 40(sp)
	sd a2, 48(sp)
	sd a3, 56(sp)
	sd a4, 64(sp)
	sd a5, 72(sp)
	sd a6, 80(sp)
	sd a7, 88(sp)
	sd t3, 96(sp)
	sd t4, 104(sp)
	sd t5, 112(sp)
	sd t6, 120(sp)

	# Call C handler: syscall_handler(a7, a0, a1, a2)
	mv a0, a7
	ld a1, 32(sp)	# Original a0
	ld a2, 40(sp)	# Original a1
	ld a3, 48(sp)	# Original a2
	call syscall_handler

	# Move mepc forward by 4 (skip the ecall instruction)
	csrr t0, mepc
	addi t0, t0, 4
	csrw mepc, t0

	# Restore registers
	ld ra, 0(sp)
	ld t0, 8(sp)
	ld t1, 16(sp)
	ld t2, 24(sp)
	ld a0, 32(sp)
	ld a1, 40(sp)
	ld a2, 48(sp)
	ld a3, 56(sp)
	ld a4, 64(sp)
	ld a5, 72(sp)
	ld a6, 80(sp)
	ld a7, 88(sp)
	ld t3, 96(sp)
	ld t4, 104(sp)
	ld t5, 112(sp)
	ld t6, 120(sp)
	addi sp, sp, 128
	mret
		
