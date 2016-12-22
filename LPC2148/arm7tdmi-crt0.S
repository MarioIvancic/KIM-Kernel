/*
 crt0.S for LPC2xxx
 - based on examples from R O Software
 - based on examples from newlib-lpc
 - based on an example from Anglia Designs

 collected and modified by Martin Thomas
*/

//#define TRAP_DEBUG

//#define __CRT0_STARTUP_DELAY 0x10000
#ifndef __CRT0_STARTUP_DELAY
#define __CRT0_STARTUP_DELAY 1
#endif

#ifndef VICVECTADDR
// default is for LPC21xx & LPC22xx with PL190 vic
#define VICVECTADDR 0xFFFFF030
#endif

	.global _etext			// .data initial values in ROM
	.global _sdata			// .data area in RAM
	.global _edata			// end of .data area
	.global _sbss    		// .bss area in RAM
	.global _ebss   		// end of .bss area
	.global _heap_start	    // heap start
	.global _stack			// top of stack
	.global _user_flash_start // ovdje se moze upisivati u flash

// Stack Sizes
	.set UND_STACK_SIZE, 0x00000004
	.set ABT_STACK_SIZE, 0x00000010
	.set FIQ_STACK_SIZE, 0x00000004
	.set IRQ_STACK_SIZE, 0x00000100
	.set SVC_STACK_SIZE, 0x00000004

// Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs
	.set MODE_USR, 0x10		// User Mode
	.set MODE_FIQ, 0x11		// FIQ Mode
	.set MODE_IRQ, 0x12		// IRQ Mode
	.set MODE_SVC, 0x13		// Supervisor Mode
	.set MODE_ABT, 0x17		// Abort Mode
	.set MODE_UND, 0x1B		// Undefined Mode
	.set MODE_SYS, 0x1F		// System Mode

	.equ I_BIT, 0x80		// when I bit is set, IRQ is disabled
	.equ F_BIT, 0x40		// when F bit is set, FIQ is disabled
	.equ T_BIT, 0x20		// when T bit is set CPU is in THUMB mode
	.equ IRQ_DISABLE, (1 << 7)
	.equ FIQ_DISABLE, (1 << 6)

	.text
	.arm
	.section .init, "ax"

	.code 32
	.align 2

	.global _start
	.func   _start
_start:

// Runtime Interrupt Vectors
// -------------------------
Vectors:
	b _boot	    			// reset - _boot
	ldr pc, _undf			// undefined - _undf
	ldr pc, _swi			// SWI - _swi
	ldr pc, _pabt			// program abort - _pabt
	ldr pc, _dabt			// data abort - _dabt
	nop						// reserved, address 0x14
    ldr pc, _irqh			// IRQ - read the VIC
    ldr pc, _fiqh			// FIQ - _fiq


// pointers
_undf:			.word 	__exit		// undefined - _exit
_swi:			.word 	__exit		// SWI - _exit
#ifndef TRAP_DEBUG
    _pabt:		.word 	__exit		// program abort
    _dabt:		.word 	__exit		// data abort
#else
    _pabt:		.word 	__pabt		// program abort
    _dabt:		.word 	__dabt		// data abort
#endif
_irqh:  .word irq_handler
_fiqh:  .word fiq_handler

_stack_ptr: 	.word 	_stack
_etext_ptr: 	.word 	_etext
_sdata_ptr: 	.word 	_sdata
_edata_ptr: 	.word 	_edata
_sbss_ptr:  	.word 	_sbss
_ebss_ptr:  	.word 	_ebss
_end_ptr:   	.word 	_end
__ctors_start__ptr: 	.word 	__ctors_start__
__ctors_end__ptr: 		.word 	__ctors_end__
_main_ptr:  	.word 	main
_VICVECTADDR_PTR: 		.word 	VICVECTADDR
STARTUP_DELAY_PTR:		.word	__CRT0_STARTUP_DELAY

#ifdef TRAP_DEBUG
_data_program_abort_context_ptr: 	.word 	data_program_abort_context
_data_program_abort_handler_ptr: 	.word 	data_program_abort_handler
#endif

// set this to points to some real FIQ handler
_C_fiq_handler_ptr: 	.word 	__exit

	.endfunc


// Setup the operating mode & stack.
// Initialize Interrupt System
// - Set stack location for each mode
// - Leave in System Mode with Interrupts Disabled
// -----------------------------------------------

	.func	_boot
_boot:

    /* delay at startup to let JTAG-connection catch-up                     */
	ldr r0, STARTUP_DELAY_PTR
1:
	subs r0, r0, #1
	bne 1b

    // init stacks

	//ldr r0, =_stack
	ldr r0, _stack_ptr
	msr CPSR_c, #MODE_UND|I_BIT|F_BIT // Undefined Instruction Mode
	mov sp, r0
	sub r0, r0, #UND_STACK_SIZE
	msr CPSR_c, #MODE_ABT|I_BIT|F_BIT // Abort Mode
	mov sp,r0
	sub r0, r0, #ABT_STACK_SIZE
	msr CPSR_c, #MODE_FIQ|I_BIT|F_BIT // FIQ Mode
	mov sp,r0
	sub r0, r0, #FIQ_STACK_SIZE
	msr CPSR_c, #MODE_IRQ|I_BIT|F_BIT // IRQ Mode
	mov sp, r0
	sub r0, r0, #IRQ_STACK_SIZE
	msr CPSR_c, #MODE_SVC|I_BIT|F_BIT // Supervisor Mode
	mov sp, r0
	sub r0, r0, #SVC_STACK_SIZE
	msr CPSR_c, #MODE_SYS|I_BIT|F_BIT // System Mode
	mov sp, r0

// Copy initialized data to its execution address in RAM
// -----------------------------------------------------
#ifdef ROM_RUN
	ldr r1, _etext_ptr		// -> ROM data start
	ldr r2, _sdata_ptr		// -> data start
	ldr r3, _edata_ptr		// -> end of data
1:
	cmp r2, r3				// check if data to move
	ldrlo r0, [r1], #4		// copy it
	strlo r0, [r2], #4
	blo 1b					// loop until done
#endif

// Clear .bss
// ----------
	mov r0, #0		     	// get a zero
	//ldr r1, =_sbss      	// -> bss start
	//ldr r2, =_ebss	    // -> bss end
	ldr r1, _sbss_ptr      	// -> bss start
	ldr r2, _ebss_ptr	    // -> bss end
2:
	cmp r1, r2				// check if data to clear
	strlo r0, [r1], #4		// clear 4 bytes
	blo 2b					// loop until done

// Clear .heap and .stack
// ----------
	ldr r1, _end_ptr	    // -> heap start
	ldr r2, _stack_ptr		// -> stack end
2:
	cmp r1, r2				// check if data to clear
	strlo r0, [r1], #4		// clear 4 bytes
	blo 2b					// loop until done

/*
Call C++ constructors (for objects in "global scope")
ctor loop added by Martin Thomas 4/2005
based on a Anglia Design example-application for ST ARM
*/

	LDR r0, __ctors_start__ptr
	LDR r1, __ctors_end__ptr
ctor_loop:
	cmp r0, r1
	beq ctor_end
	ldr r2, [r0], #4
	stmfd sp!, {r0-r1}
	mov lr, pc
	mov pc, r2
	ldmfd sp!, {r0-r1}
	b ctor_loop
ctor_end:

// Call main program: main(0)
// --------------------------
	mov r0,#0				// no arguments (argc = 0)
	mov r1,r0
	mov r2,r0
	mov fp,r0				// null frame pointer
	mov r7,r0				// null frame pointer for thumb
	ldr r10, _main_ptr
	mov lr,pc

/* Enter the C code, use BX instruction so as to never return */
/* use BLX (?) main if you want to use c++ destructors below */

	bx r10		 			// enter main()

	// nakon povratka iz main propada se na reset
	.size   _start, . - _start
	.endfunc


	.global abort
	.global __exit
	.func __exit
abort:
__exit:
// disable interrupts and enter endless loop

	mrs r0,cpsr				// get PSR
	orr r0, r0, #(I_BIT | F_BIT)	// disable IRQ & FIQ
   	msr cpsr, r0			// set up status register
	b .
	.endfunc


// FIQ ISR
    .func fiq_handler
fiq_handler:
	sub lr, lr, #4
	stmfd sp!, {r0-r4, ip, lr}
	mrs lr, spsr
	tst lr, #F_BIT
	ldmnefd sp!, {r0-r4, ip, pc}^

	// poziva se C handler, ARM ili Thumb
	ldr r4, _C_fiq_handler_ptr
	mov lr, pc
	bx r4

	ldmfd sp!, {r0-r4, ip, pc}^
	.endfunc

#define NESTED_IRQ

#ifdef NESTED_IRQ
// nested irq handler
    .func irq_handler
irq_handler:
// Save IRQ context, including the APCS registers, and r4-6
	sub lr, lr, #4
	stmfd sp!, {r0-r6, ip, lr}

	// Save the SPSR_irq register
	mrs r4, spsr
	tst lr, #I_BIT
	ldmnefd sp!, {r0-r6, ip, pc}^

	// Read the VICVectAddr
	ldr r5, _VICVECTADDR_PTR
	ldr r6, [r5]

#ifdef IRQ_DEBUG
	// Change to SYS mode and disable IRQ
	msr cpsr_c, #MODE_SYS|IRQ_DISABLE
#else // ! IRQ_DEBUG
	// Change to SYS mode and enable IRQ
	msr cpsr_c, #MODE_SYS
#endif // IRQ_DEBUG

	// Save the banked SYS mode link register
	stmfd sp!, {lr}

	// Call the C-coded handler
	mov lr, pc
	bx r6

	// Restore SYS mode link register
	ldmfd sp!, {lr}

	// Change to IRQ mode and disable IRQ
	msr cpsr_c, #MODE_IRQ|IRQ_DISABLE

	// Restore the SPSR
	msr spsr, r4

	// Acknowledge the VIC
	mov r0, #0
	str r0, [r5]

	// Restore IRQ context and return from interrupt
	ldmfd sp!, {r0-r6, ip, pc}^
	.endfunc

#else


// klasicni IRQ handler
    .func irq_handler
irq_handler:
	sub lr, lr, #4
	stmfd sp!, {r0-r5, ip, lr}

// ako je IRQ bio zabranjen prije IRQ-a ne radi se nista
	mrs lr, spsr
	tst lr, #I_BIT

	ldmnefd sp!, {r0-r5, ip, pc}^

	ldr r4, _VICVECTADDR_PTR
	ldr r5, [r4] 			// cita se adresa C hadler-a za dati IRQ

	mov lr, pc
	bx r5					// poziva se C handler, ARM ili Thumb

	// Acknowledge the VIC
	mov r0, #0
	str r0, [r4]

	// return from ISR
	ldmfd sp!, {r0-r5, ip, pc}^
	.endfunc

#endif


	.global __end_of_CRT0S__
__end_of_CRT0S__:

// malo prostora za serijski broj - treba 19 bajta, rezervise se 32
// serijski broj je NULL-terminirani string od max. 30 karaktera.
// karakter iza terminacionog NULL karaktera je CRC8 prethodih karaktera
	.org 0x01dc
	.global str_serial_number
str_serial_number:
	.space 32, 0

// code read protection: na adresi 0x01fc treba upisati 0x87654321
	.global code_read_protection_word
code_read_protection_word:
	.word 0x87654321		// turn ON code read protection


#ifdef TRAP_DEBUG
    .func __pabt
__pabt:
	ldr sp, =(data_program_abort_context + 5*4) // set sp_abt to data array with offset (restore later)
	stmia sp!, {r0-r12}			// save first dataset in r0-r12 registers to array
	mov r1, #0					// flag for program abort
	stmia sp!, {r1}

	sub r0, lr, #4				// calculate PC value of pabort instruction: r0 = lr - 4
	b 1f
	.endfunc

    .func __dabt
__dabt:
	ldr sp, =(data_program_abort_context + 5*4) // set sp_abt to data array with offset (restore later)
	stmia sp!, {r0-r12}			// save first dataset in r0-r12 registers to array
	mov r1, #1					// flag for data abort
	stmia sp!, {r1}

	sub r0, lr, #8				// calculate PC value of dabort instruction: r0 = lr - 8

1:
	mrs r5, cpsr				// save current mode to r5 for mode switching
	mrs r6, spsr				// spsr_abt = CPSR of dabt originating mode: save to r6 for mode switching

	mov r2, r6					// building second dataset: r2 = CPSR (of exception)

	tst r6, #0xf				// test for the mode which raised exception
	orreq r6, r6, #0xf			// if EQ => change mode usr->sys; else do nothing

	bic r7, r6, #T_BIT			// go to forced ARM state via r7
	msr cpsr_c, r7				// switch out from mode 0x17 (ABT) to dabt generating mode and state

	mov r3, lr					// get lr (= r3)
	mov r4, sp					// get sp (= r4)

	msr cpsr_c, r5				// switch back to mode 0x17 (ABT)

	tst r1, #1					// is this data or program abort?
	beq 1f						// program abort, skip instruction fetch

	tst r6, #T_BIT				// THUMB state raised exception?
	ldrneh r1, [r0]				// yes, THUBM, load 16-bit instruction: r1 = [pc](dabt)
	ldreq r1, [r0]				// no, ARM, load 32-bit instruction: r1 = [pc](dabt)

1:
	ldr sp, _data_program_abort_context_ptr	// reset sp to array's starting address
	stmia sp, {r0-r4}			// and save the 2nd dataset from r0 to r4

	// clean up & restoration follows: restored full context, sp first
	ldr sp, =(_stack - UND_STACK_SIZE) // sp is on the top of ABT stack

	ldr r5, _data_program_abort_handler_ptr
	mov lr, pc
	bx r5						// call data_program_abort_handler (ARM or THUMB)

	b .							// loop forever
	.endfunc
#endif // TRAP_DEBUG

	.end
