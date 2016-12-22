/*
 crt0.S for LPC2xxx
 - based on examples from R O Software
 - based on examples from newlib-lpc
 - based on an example from Anglia Designs

 collected and modified by Martin Thomas
 further modified by Mario Ivančić
*/

#define NESTED_IRQ
//#define __CRT0_STARTUP_DELAY 0x10000
#ifndef __CRT0_STARTUP_DELAY
#define __CRT0_STARTUP_DELAY 1
#endif

#ifndef VICVECTADDR
// default is for LPC21xx & LPC22xx with PL190 vic
#define VICVECTADDR 0xFFFFF030
#endif

	// symbols from linked script
	.global _etext			// .data initial values in ROM
	.global _sdata			// .data area in RAM
	.global _edata			// end of .data area
	.global _sbss    		// .bss area in RAM
	.global _ebss   		// end of .bss area
	.global _heap_start	    // heap start
	.global _stack			// top of stack
	.global _user_flash_start // ovdje se moze upisivati u flash

// Stack Sizes
	.set UND_STACK_SIZE, 0x00000004		// we are placing UND stack above ABT stack so in UND mode we will overwrite ABT stack
	.set ABT_STACK_SIZE, 0x00000100		// 4 is enough if not processing aborts
	.set FIQ_STACK_SIZE, 0x00000010		// this is enough for dummy C FIQ handler, you may increase it
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
	b _boot	    	        // reset - _boot
	ldr pc, __undf_ptr		// undefined - __undf
	ldr pc, __swi_ptr		// SWI - __swi
	ldr pc, __pabt_ptr		// program abort __pabt
	ldr pc, __dabt_ptr		// data abort __dabt
	nop						// reserved, address 0x14
    ldr pc, __irq_ptr		// IRQ - read the VIC
    ldr pc, __fiq_ptr		// FIQ - _fiq
    
// pointers, can point to any memory region    
__undf_ptr: .word __undf        // undefined instruction
__swi_ptr:  .word __swi         // swi
__pabt_ptr: .word __pabt		// program abort
__dabt_ptr: .word __dabt		// data abort
__irq_ptr:  .word   irq_asm_trampoline
__fiq_ptr:  .word   fiq_asm_trampoline
    .endfunc
    
    .weak fiq_handler
    .set fiq_handler, __do_nothing_and_return
    
    .weak __swi
    .set __swi, __do_nothing_and_return

    // FIQ ASM trampoline, can be in flash or in ram
fiq_asm_trampoline:
	sub lr, lr, #4
    
    // according to ATPCS we have to preserve r0-r3, r12 (ip) and lr
    // but r8 - r12 are banked so we don't have to save r12
    
	stmfd sp!, {r0-r3, lr}
	
	// Save the SPSR_fiq register
	// if FIQ was disabled before entering this handler just return
	// this is needed because interrupt disable operation is not atomic on arm7tdmi
    
	mrs lr, spsr
	tst lr, #F_BIT
	ldmnefd sp!, {r0-r3, pc}^
	
	// call C FIQ handler, ARM or Thumb, using FIQ stack
	ldr r12, =fiq_handler
	mov lr, pc
	bx r12
	ldmfd sp!, {r0-r3, pc}^
	.endfunc




// Setup the operating mode & stack.
// Initialize Interrupt System
// - Set stack location for each mode
// - Leave in System Mode with Interrupts Disabled
// -----------------------------------------------

	.func	_boot
_boot:

    // delay at startup to let JTAG-connection catch-up
	ldr r0, =__CRT0_STARTUP_DELAY
1:
	subs r0, r0, #1
	bne 1b

    // init stacks

	ldr r0, =_stack
	
	msr CPSR_c, #MODE_UND|I_BIT|F_BIT // Undefined Instruction Mode
	mov sp, r0
	sub r0, r0, #UND_STACK_SIZE
	
	msr CPSR_c, #MODE_ABT|I_BIT|F_BIT // Abort Mode
	mov sp, r0
	sub r0, r0, #ABT_STACK_SIZE
	
	msr CPSR_c, #MODE_FIQ|I_BIT|F_BIT // FIQ Mode
	mov sp, r0
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
	ldr r1, =_etext			// -> ROM data start
	ldr r2, =_sdata			// -> data start
	ldr r3, =_edata			// -> end of data
1:
	cmp r2, r3				// check if data to move
	ldrlo r0, [r1], #4		// copy it
	strlo r0, [r2], #4
	blo 1b					// loop until done
#endif

// Clear .bss
// ----------
	mov r0, #0		     	// get a zero
	ldr r1, =_sbss      	// -> bss start
	ldr r2, =_ebss	    	// -> bss end
2:
	cmp r1, r2				// check if data to clear
	strlo r0, [r1], #4		// clear 4 bytes
	blo 2b					// loop until done

// Clear .heap and .stack
// ----------
	ldr r1, =_end	    	// -> heap start
	ldr r2, =_stack			// -> stack end
2:
	cmp r1, r2				// check if data to clear
	strlo r0, [r1], #4		// clear 4 bytes
	blo 2b					// loop until done

/*
Call C++ constructors (for objects in "global scope")
ctor loop added by Martin Thomas 4/2005
based on a Anglia Design example-application for ST ARM
*/

	LDR r0, =__ctors_start__
	LDR r1, =__ctors_end__
1:
	cmp r0, r1
	beq 1f
	ldr r2, [r0], #4
	stmfd sp!, {r0-r1}
	mov lr, pc
	mov pc, r2
	ldmfd sp!, {r0-r1}
	b 1b
1:

// Call main program: main(0)
// --------------------------
	mov r0,#0				// no arguments (argc = 0)
	mov r1,r0
	mov r2,r0
	mov fp,r0				// null frame pointer
	mov r7,r0				// null frame pointer for thumb
    
    /* Enter the C code */
	ldr r10, =main
	// mov lr,pc
    ldr lr, =__exit
	bx r10		 			// enter main()

	// in case main exits we fall-through to __exit
	.size   _boot, . - _boot
	.endfunc


	.global abort
	.global __exit
	.func __exit
abort:
__exit:
// disable interrupts and enter endless loop

	mrs r0, cpsr					// get PSR
	orr r0, r0, #(I_BIT | F_BIT)	// disable IRQ & FIQ
   	msr cpsr_c, r0					// set up status register
	b .
	.endfunc


// default do-nothing-and-return function
	.type   __do_nothing_and_return, %function
	.func __do_nothing_and_return
__do_nothing_and_return:
	bx lr
	.endfunc
	
	
   

    .func irq_asm_trampoline
    
#ifdef NESTED_IRQ_HANDLER
irq_asm_trampoline:
    // 25 instructions, 4 words on IRQ stack, 6 words on SYS stack, 11 loads, 9 stores (20 LoadStore)
	// prepare return address
	SUB LR, LR, #4
	
	// Registers to be used in the ISR should be stacked along with SPSR_irq and LR_irq
	STMFD SP!, {R0, LR}
	MRS R0, SPSR
	
	// if IRQ was disabled before entering this handler just return
	// this is needed because interrupt disable operation is not atomic on arm7tdmi
	TST R0, #I_BIT
	LDMNEFD SP!, {R0, PC}^
	
	// stack SPRS_irq and R1
	STMFD SP!, {R0, R1}
	
	// Move to System mode and re-enable interrupts
	// MSR cpsr_c, #0x1f
    MRS r1, cpsr
    BIC r1, r1, #I_BIT
    ORR r1, r1, #MODE_SYS
    MSR cpsr_c, r1
	
// -------------------- IRQ ENABLED, SYS -----------------------------
	
	// Load VICVECTADDR in R0 and vector in R1
	LDR R0, =VICVECTADDR
	LDR R0, [R0]

	// Stack lr_sys and other register (the APCS registers)
	STMFD SP!, {R2, R3, R12, LR}

	// Branch to C function (ARM or Thumb)
	MOV LR, PC
	BX R0
	
	// Pop lr_sys and ATPCS registers
	LDMFD SP!, {R2, R3, R12, LR}

	// Move to IRQ and disable interrupts. For considering the scenario that an interrupt
	// occurs while IRQ is disabled please refer the ARM FAQ online.
	// MSR cpsr_c, #0x92
    MRS r1, cpsr
    bic r1, r1, #MODE_SYS
    orr r1, r1, #MODE_IRQ | I_BIT
    MSR cpsr_c, r1
	
// -------------------- IRQ DISABLED, IRQ -----------------------------

	// Update VIC;
    LDR R0, =VICVECTADDR
	STR R1, [R0]

	// Pop registers, SPSR_irq, R1
	LDMFD SP!, {R0, R1}
	MSR SPSR_cxsf, R0

	// Return
	LDMFD SP!, {R0, PC}^
	
#else // ! NESTED_IRQ_HANDLER

// clasical (not-nested) IRQ handler

irq_asm_trampoline:
    // 12 instructions, 6 words + C function stack on IRQ stack, 9 loads, 7 stores (16 LoadStore)
	// prepare return address
	sub lr, lr, #4
	
    // Save IRQ context, including the APCS registers, and r4-r5
	stmfd sp!, {r0-r3, r12, lr}

	// Save the SPSR_irq register
	// if IRQ was disabled before entering this handler just return
	// this is needed because interrupt disable operation is not atomic on arm7tdmi
	mrs lr, spsr
	tst lr, #I_BIT
	ldmnefd sp!, {r0-r3, r12, pc}^

	// load real handler address for this IRQ reguest from VIC
	ldr r1, =VICVECTADDR
	ldr r0, [r1]

	// call the C handler function, ARM ili Thumb, using IRQ stack
	mov lr, pc
	bx r0

	// Acknowledge the VIC (not important what r0 contains)
    ldr r1, =VICVECTADDR
	str r0, [r1]

	// return from ISR
	ldmfd sp!, {r0-r2, r12, pc}^
#endif // NESTED_IRQ
    .endfunc


	.global __end_of_CRT0S__
__end_of_CRT0S__:

// 32 bytes for serial number, patched at programing time
	.org 0x01dc
	.global str_serial_number
str_serial_number:
	.space 32, 0

// code read protection:
// to enable word at address 0x01fc should be 0x87654321
// to disable word at address 0x01fc should be 0x00000000
// this also can be patched at programing time
	.global code_read_protection_word
code_read_protection_word:
	.word 0x87654321		// turn ON code read protection


// fault exception (program or data abort) and undefined instruction handler

	// C abort handler function declared weak to make it optional
	.weak data_program_abort_handler
    .set data_program_abort_handler, __do_nothing_and_return
    
    .func __undf
__undf:
	// sp points to top of UND stack
	// save r0-r12, lr to UND-ABT stack
	// Note: UND stack will corrupt ABT stack, but we have no plans to return
	// from this handler so it's not a issue
	stmfd sp!, {r0-r12, lr}
	
	// save current mode to r5 for mode switching and disable IRQ and FIQ
	mrs r5, cpsr
	orr r5, r5, #I_BIT | F_BIT
	msr cpsr_c, r5
	
	// set flag for undefined instruction in r1
	mov r1, #2
	stmfd sp!, {r1}
	// address of undefined instruction: lr - 4
	sub r0, lr, #4
	// now go to common ABT code
	b 1f
	.endfunc

    .func __pabt
__pabt:
	// sp points to top of ABT stack
	// save r0-r12, lr to ABT stack
	stmfd sp!, {r0-r12, lr}
	
	// save current mode to r5 for mode switching and disable IRQ and FIQ
	mrs r5, cpsr
	orr r5, r5, #I_BIT | F_BIT
	msr cpsr_c, r5
	
	// set flag for program abort in r1
	mov r1, #0
	stmfd sp!, {r1}
	// address of pabort instruction: lr - 4
	sub r0, lr, #4
	// now go to common ABT code
	b 1f
	.endfunc

    .func __dabt
__dabt:
	// sp points to top of ABT stack
	// save r0-r12 to ABT stack
	stmfd sp!, {r0-r12}
	
	// save current mode to r5 for mode switching and disable IRQ and FIQ
	mrs r5, cpsr
	orr r5, r5, #I_BIT | F_BIT
	msr cpsr_c, r5
	
	// set flag for data abort in r1
	mov r1, #1
	stmfd sp!, {r1}
	// address of dabort instruction: lr - 8
	sub r0, lr, #8

	// common ABT code
1:
	mrs r6, spsr				// spsr_abt = CPSR of dabt originating mode: save to r6 for mode switching

	mov r2, r6					// building second dataset: r2 = CPSR (of exception)

	tst r6, #0xf				// test for the mode which raised exception
	orreq r6, r6, #0xf			// if mode == USR change it to sys; else do nothing

	bic r7, r6, #T_BIT			// go to forced ARM state via r7
	msr cpsr_c, r7				// switch out from mode 0x17 (ABT) to dabt generating mode and ARM state

	mov r3, lr					// get lr (= r3)
	mov r4, sp					// get sp (= r4)

	msr cpsr_c, r5				// switch back to mode 0x17 (ABT)

	cmp r1, #0					// is this program (prefetch) abort?
	beq 1f						// program abort, skip instruction fetch

	tst r6, #T_BIT				// THUMB state raised exception?
	ldrneh r1, [r0]				// yes, THUBM, load 16-bit instruction: r1 = [pc](dabt)
	ldreq r1, [r0]				// no, ARM, load 32-bit instruction: r1 = [pc](dabt)

1:
	stmfd sp, {r0-r4}			// save instruction address, instruction opcode, original CPRS, original LR, original SP
	// now move sp to r0 so r0 will point to stack frame with all data in it
	// stack frame: PC, OPCODE, CPSR, LR, SP, FLAG, R0, .., R12
	mov r0, sp

	ldr r5, =data_program_abort_handler
	mov lr, pc
	bx r5						// call data_program_abort_handler (ARM or THUMB)

	b .							// loop forever
	.endfunc

	.end
