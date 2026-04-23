#include "hardware.h"

.section .text.init
.global _start

_start:
	csrr t0, mhartid	# Read Machine Hardware thread ID
	bnez t0, park		# If not primary core, park

	la t0, _bss_start
	la t1, _bss_end
bss_loop:
	bgeu t0, t1, bss_done
	sd zero, 0(t0)
	addi t0, t0, 8
	j bss_loop

bss_done:
	la sp, _stack_top
	la t0, trap_handler
	csrw mtvec, t0
	
	# INITIALIZE MSCRATCH: Points to our trap frame memory
	la t0, main_trap_frame
	csrw mscratch, t0

	call kmain

park:
	wfi
	j park

.align 4
trap_handler:
	# 1. SWAP: sp now points to our trap_frame_t, mscratch holds User SP
	csrrw sp, mscratch, sp
	
	# 2. SAVE x5 (t0) first, then use it to move User SP from mscratch to memory
	sd x5,	40(sp)
	csrr x5, mscratch
	sd x5,	16(sp)   # Save User SP into xregs[2] slot

	# 3. SAVE remaining x-registers
	sd x1,  8(sp)
	sd x3,  24(sp)
	sd x4,  32(sp)
	# x5 is already at 40
	sd x6,  48(sp)
	sd x7,  56(sp)
	sd x8,  64(sp)
	sd x9,  72(sp)
	sd x10, 80(sp)
	sd x11, 88(sp)
	sd x12, 96(sp)
	sd x13, 104(sp)
	sd x14, 112(sp)
	sd x15, 120(sp)
	sd x16, 128(sp)
	sd x17, 136(sp)
	sd x18, 144(sp)
	sd x19, 152(sp)
	sd x20, 160(sp)
	sd x21, 168(sp)
	sd x22, 176(sp)
	sd x23, 184(sp)
	sd x24, 192(sp)
	sd x25, 200(sp)
	sd x26, 208(sp)
	sd x27, 216(sp)
	sd x28, 224(sp)
	sd x29, 232(sp)
	sd x30, 240(sp)
	sd x31, 248(sp)

	# 4. SAVE CSRs
	csrr t0, mepc
	sd t0,	256(sp)
	csrr t0, mstatus
	sd t0,	264(sp)

	# 5. CALL C HANDLER: syscall_handler(num, arg0, arg1, arg2)
	# Mapping: a7=x17, a0=x10, a1=x11, a2=x12
	ld a0,	136(sp)
	ld a1,	80(sp)
	ld a2,	88(sp)
	ld a3,	96(sp)
	call syscall_handler

	# 6. RESTORE CSRs
	ld t0, 256(sp)
	addi t0, t0, 4      # Skip ecall
	csrw mepc, t0
	ld t0, 264(sp)
	csrw mstatus, t0

	# 7. PREPARE USER SP
	ld t0, 16(sp)
	csrw mscratch, t0

	# 8. RESTORE GPRs
	ld x1,  8(sp)
	ld x3,  24(sp)
	ld x4,  32(sp)
	# Skip x5 (t0) for now
	ld x6,  48(sp)
	ld x7,  56(sp)
	ld x8,  64(sp)
	ld x9,  72(sp)
	ld x10, 80(sp)
	ld x11, 88(sp)
	ld x12, 96(sp)
	ld x13, 104(sp)
	ld x14, 112(sp)
	ld x15, 120(sp)
	ld x16, 128(sp)
	ld x17, 136(sp)
	ld x18, 144(sp)
	ld x19, 152(sp)
	ld x20, 160(sp)
	ld x21, 168(sp)
	ld x22, 176(sp)
	ld x23, 184(sp)
	ld x24, 192(sp)
	ld x25, 200(sp)
	ld x26, 208(sp)
	ld x27, 216(sp)
	ld x28, 224(sp)
	ld x29, 232(sp)
	ld x30, 240(sp)
	ld x31, 248(sp)

	# 9. RESTORE BRIDGE AND SWAP
	ld x5, 40(sp)
	csrrw sp, mscratch, sp
	mret

.section .data
.align 8
main_trap_frame:
	.space 272 # (32 registers + mepc + mstatus) * 8 bytes
