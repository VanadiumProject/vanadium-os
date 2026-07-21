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

	# 4. SAVE CSRs (t0 keeps mcause for the dispatch below)
	csrr t0, mepc
	sd t0,	256(sp)
	csrr t0, mstatus
	sd t0,	264(sp)
	csrr t1, mtval
	sd t1,	280(sp)
	csrr t0, mcause
	sd t0,	272(sp)

	# 5. DISPATCH ON MCAUSE
	# Bit 63 set => interrupt, clear => exception. Only the syscall path
	# may advance mepc: an interrupt's mepc already points at the
	# instruction that still has to run.
	bltz t0, trap_interrupt

	li t1, 11		# ecall from M-mode
	beq t0, t1, trap_syscall
	li t1, 8		# ecall from U-mode
	beq t0, t1, trap_syscall
	j trap_fatal

trap_syscall:
	# 6. CALL C HANDLER: syscall_handler(num, arg0, arg1, arg2)
	# Mapping: a7=x17, a0=x10, a1=x11, a2=x12
	ld a0,	136(sp)
	ld a1,	80(sp)
	ld a2,	88(sp)
	ld a3,	96(sp)
	call syscall_handler
	sd a0,	80(sp)		# Return value lands in the caller's a0

	# 7. STEP OVER THE ECALL
	ld t0, 256(sp)
	addi t0, t0, 4
	sd t0, 256(sp)
	j trap_return

trap_interrupt:
	# Nothing enables interrupts yet (mie and mstatus.MIE are untouched),
	# so anything landing here is unexpected. Timer/external dispatch
	# hooks in here once the CLINT is up.
	j trap_fatal

trap_fatal:
	# trap_fatal_handler(mcause, mepc, mtval) never returns
	ld a0, 272(sp)
	ld a1, 256(sp)
	ld a2, 280(sp)
	call trap_fatal_handler

trap_return:
	# 8. RESTORE CSRs
	ld t0, 256(sp)
	csrw mepc, t0
	ld t0, 264(sp)
	csrw mstatus, t0

	# 9. PREPARE USER SP
	ld t0, 16(sp)
	csrw mscratch, t0

	# 10. RESTORE GPRs
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

	# 11. RESTORE BRIDGE AND SWAP
	ld x5, 40(sp)
	csrrw sp, mscratch, sp
	mret

.section .bss
.align 4

# The trap frame sits at the TOP of this stack on purpose. The handler runs
# with sp = main_trap_frame, so any C function it calls pushes its frame down
# into trap_stack instead of over whatever happens to sit below us in memory.
trap_stack:
	.space 4096
main_trap_frame:
	.space 296 # (32 registers + mepc + mstatus + mcause + mtval + kernel_sp) * 8 bytes
		   # Must stay in sync with trap_frame_t in include/types.h
