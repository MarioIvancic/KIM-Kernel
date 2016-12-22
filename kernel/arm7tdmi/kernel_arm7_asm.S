// kernel_arm7_asm.S
// ARM ASM functions

// Stack Sizes (from crt0.S)
    .set UND_STACK_SIZE, 0x00000004        // we are placing UND stack above ABT stack so in UND mode we will overwrite ABT stack
    .set ABT_STACK_SIZE, 0x00000100        // 4 is enough if not processing aborts
    .set FIQ_STACK_SIZE, 0x00000010        // this is enough for dummy C FIQ handler, you may increase it
    .set IRQ_STACK_SIZE, 0x00000100
    .set SVC_STACK_SIZE, 0x00000010

// Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs
    .set MODE_USR, 0x10        // User Mode
    .set MODE_FIQ, 0x11        // FIQ Mode
    .set MODE_IRQ, 0x12        // IRQ Mode
    .set MODE_SVC, 0x13        // Supervisor Mode
    .set MODE_ABT, 0x17        // Abort Mode
    .set MODE_UND, 0x1B        // Undefined Mode
    .set MODE_SYS, 0x1F        // System Mode

    .equ I_BIT, 0x80        // when I bit is set, IRQ is disabled
    .equ F_BIT, 0x40        // when F bit is set, FIQ is disabled
    .equ T_BIT, 0x20        // when T bit is set CPU is in THUMB mode
    

    .text
    .arm
    .code 32
    .align 2
    
    
        .func irq_asm_trampoline
irq_asm_trampoline:
sys_stack_save_restore8:
// note: kernel_context_switcher is called from this function
// 15 instructions, 1 word + C of IRQ stack, 6 words USR/SYS stack, 14 loads, 10 stores, context on stack (IRQ, no context switching)
// 34 instructions, 1 word + C of IRQ stack, 16 words USR/SYS stack, 28 loads, 21 stores, context on stack (IRQ and context switching)
// About 43 cycles if no context switch, about 90 cycles for context switch (1.5us @ 60MHz)
	// prepare return address (IRQ only, not needed for SVC)
	sub lr, lr, #4
	
	// get USR/SYS sp in sp_irq without mode switching
    // stmdb sp!, {sp}^
    // ldr sp, [sp]
    stmdb sp, {sp}^
    ldr sp, [sp, #-4]
    
    // now we have USR/SYS sp in sp_irq
    // save r0-r5, r12, lr_irq to USR/SYS stack using sp_irq
    stmdb sp!, {r0-r5, r12, lr}
	
	// we will use r5 as USR/SYS sp
	mov r5, sp
    
    // restore sp_irq
    ldr sp, =_stack
    sub sp, sp, #(UND_STACK_SIZE + ABT_STACK_SIZE + FIQ_STACK_SIZE)
    
    // load real handler address for this IRQ reguest from VIC
	ldr r4, =VICVECTADDR
	ldr r0, [r4]
	
	// call the C handler function, ARM or Thumb, using IRQ stack, interrupts disabled
	mov lr, pc
	bx r0

	// Acknowledge the VIC (not important what r0 contains)
	str r0, [r4]
	
	// test and clear kernel_do_cs
    ldr r0, =kernel_do_cs
	ldr r1, [r0]
	cmp r1, #0
	
    // if kernel_do_cs is 0 just return
	// but first restore saved registers and then return to user task
	ldmeqia r5, {r0-r5, r12, pc}^
    
    // clear kernel_do_cs
    mov r1, #0
	str r1, [r0]
	
	// we have to do context switch but first we have to save some more regs
	// we have to save r6-r11, lr_sys, spsr_irq
	mrs r3, spsr
	stmdb r5!, {r3, r6-r11, lr}^
	
	// now we have to copy sp_sys to TCB
	
	// get pointer to sp in TCB
    ldr r4, =kernel_current_task
    ldr r1, [r4]
	
	// store sp
	str r5, [r1]
	
	// now we have to find next task to run and restore context of new task
    // from it's TCB
    ldr r1, =kernel_context_switcher
    mov lr, pc
    bx r1
	
	// get sp from kernel_current_task->sp
    ldr r1, [r4]
	ldr sp, [r1]
	
	// restore spsr_irq r6-r11 and lr_sys from stack
	ldmia sp!, {r3, r6-r11, lr}^
	msr spsr_cxsf, r3
	
	// restore r0-r5, r12 and lr_irq from stack
    ldmia sp!, {r0-r5, r12, lr}
	
	// set USR/SYS sp from sp_irq without mode switching
    // this is tricky part!!!
    // in sp_irq we value of sp_sys and we have to copy it to sp_sys
    // so, we first store sp_irq value to [sp_irq - 4]
    str sp, [sp, #-4]
    
    // and then we load it to sp_sys
    ldmdb sp, {sp}^
    
    // restore sp_irq
    ldr sp, =_stack
    sub sp, sp, #(UND_STACK_SIZE + ABT_STACK_SIZE + FIQ_STACK_SIZE)
    
    // return to USR/SYS mode
    movs pc, lr
	.endfunc



    .extern SVC_HandlerC


    // SWI_Handler that use SVC stack to call C SVC handler
    .func SWI_Handler
    .global SWI_Handler
SWI_Handler:
// 30 instructions, 1 word on SVC stack, 16 words on caller stack (stack frame), 25 loads, 19 stores (no context switching)
// 34 instructions, 1 word on SVC stack, 16 words on caller stack (stack frame), 28 loads, 21 stores (context switching)
    
    // get USR/SYS sp in sp_svc without mode switching
    stmdb sp, {sp}^
    ldr sp, [sp, #-4]
    
    // now we have USR/SYS sp in sp_svc
    // save r0-r5, r12, lr_irq to USR/SYS stack using sp_svc
    stmdb sp!, {r0-r5, r12, lr}
    
    // we have to save r6-r11, lr_sys, spsr_svc
    mrs r3, spsr
    stmdb sp!, {r3, r6-r11, lr}^
    
    // now we have to copy sp_sys to TCB
    
    // get pointer to sp in TCB
    ldr r4, =kernel_current_task
    ldr r1, [r4]
    
    // store sp
    str sp, [r1]
    
    // copy sp to r0 (to be arg for SVC_HandlerC) and to r5 (for later use)
    mov r0, sp
    
    // restore sp_svc
    ldr sp, =_stack
    sub sp, sp, #(UND_STACK_SIZE + ABT_STACK_SIZE + FIQ_STACK_SIZE + IRQ_STACK_SIZE)
    
    // SVC stack is now empty and all regs are saved on original stack
    // r0 points to stack frame on USR/SYS stack
    
    // call C SWI handler (ARM or Thumb)
    ldr r1, =SVC_HandlerC
    mov lr, pc
    bx r1
    
    // do nothing with kernel_do_cs (it's not used in SVC handler)
    // now we have to restore context of new task from it's TCB
    
    // get pointer to sp in TCB of kernel_current_task
    ldr r2, [r4]
    ldr sp, [r2]
    
    // restore spsr_svc r6-r11 and lr_sys from stack
    ldmia sp!, {r3, r6-r11, lr}^
    msr spsr_cxsf, r3
    
    // restore r0-r5, r12 and lr_svc from stack
    ldmia sp!, {r0-r5, r12, lr}
    
    // set USR/SYS sp from sp_svc without mode switching
    // this is tricky part!!!
    // in sp_svc we value of sp_sys and we have to copy it to sp_sys
    // so, we first store sp_svc value to [sp_svc - 4]
    str sp, [sp, #-4]
    
    // and then we load it to sp_sys
    ldmdb sp, {sp}^
    
    // restore sp_svc
    ldr sp, =_stack
    sub sp, sp, #(UND_STACK_SIZE + ABT_STACK_SIZE + FIQ_STACK_SIZE + IRQ_STACK_SIZE)
    
    // return to USR/SYS mode
    movs pc, lr
    .endfunc
    
    
    .func _kernel_get_cpsr
    .global _kernel_get_cpsr
_kernel_get_cpsr:
    mrs r0, cpsr
    bx lr
    .endfunc
    
    
    
    .func kernel_enter_critical
    .global kernel_enter_critical
    
kernel_enter_critical:
    
    // .equ I_BIT, 0x80        // when I bit is set, IRQ is disabled
    // .equ F_BIT, 0x40        // when F bit is set, FIQ is disabled
    // void kernel_enter_critical(atomic_t* state)
    // this is called from SYS mode (privileged code)
    // just disable interrupts
    // FIQ status is unchanged

    mrs r1, cpsr            // get PSR
    str r1, [r0]            // save original flags
    orr r1, r1, #I_BIT      // disable IRQ
    msr cpsr_c, r1          // set up status register
    bx lr
    .endfunc
    

    
    .func kernel_exit_critical
    .global kernel_exit_critical
    
kernel_exit_critical:

    // SCV_exit_critical();
    // this is called from SYS mode (privileged code)
    // just restore interrupt disable flag
    // FIQ status is unchanged
    ldr r1, [r0]            // get saved state
    and r1, r1, #I_BIT      // mask I_BIT
    mrs r0, cpsr            // get PSR
    bic r0, r0, #I_BIT      // clear I_BIT
    orr r0, r0, r1          // restore I_BIT from saved state
    msr cpsr_c, r0          // set up status register
    bx lr
    .endfunc
