/* irq.S -- libmcu_restore_interrupts, libmcu_disable_interrupts */

	.equ I_BIT, 0x80		// when I bit is set, IRQ is disabled
	.equ F_BIT, 0x40		// when F bit is set, FIQ is disabled

	.text
	.arm

	.code 32
	.align 2


// funkcija koja zabranjuje sve prekide
// prototip: int _disable_all(void);

	.global libmcu_disable_interrupts
	.func libmcu_disable_interrupts

libmcu_disable_interrupts:
	mrs r0, cpsr					// get PSR
   	orr	r1, r0, #I_BIT				// disable IRQ
   	msr	cpsr, r1
   	orr	r1, r1, #F_BIT				// disable FIQ
   	msr	cpsr, r1
   	bx lr
	.endfunc

// funkcija koja dozvoljava sve prekide
// u stvari radi restore prethodnog stanja
// prototip: void _enable_all(int original_state); to restore, or
// void _enable_all(0); to enable

	.global libmcu_restore_interrupts
	.func libmcu_restore_interrupts

libmcu_restore_interrupts:
	mrs r1, cpsr				// get PSR
	bic r1, r1, #I_BIT|F_BIT	// clear I and F bits
	and r0, r0, #I_BIT|F_BIT	// clear all other bits in saved mask
	orr r0, r0, r1				// copy old I and F bits to current cpsr
	msr cpsr, r0				// restore cpsr
	bx lr
	.endfunc

	.end
