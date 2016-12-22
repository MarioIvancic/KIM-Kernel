    // atomic operations for arm7tdmi

	.text
	.arm

	.code 32
	.align 2
	
	.equ I_BIT, 0x80		// when I bit is set, IRQ is disabled
	.equ F_BIT, 0x40		// when F bit is set, FIQ is disabled
	
	
	.global __spinlock_trylock
	.func __spinlock_trylock
    // int __spinlock_trylock(int *addr)
    // returns 0 for success, 1 otherwise
__spinlock_trylock:
    // addr in r0, used registers r1, r2, r3
    mov r2, #1
    swp r1, r2, [r0]        // r1 = *addr; *addr = *addr + value
    cmp r1, r2              // *addr == 1
    movne r0, #0            // if successful return 0
    moveq r0, #1            // if not successful return 1
    bx lr                   // return
    .endfunc
 


	.global __spinlock_unlock
	.func __spinlock_unlock
    // void __spinlock_unlock(int *addr)
__spinlock_unlock:
    // addr in r0, used registers r1, r2, r3
    mov r2, #0              // load UNLOCKED value
    str r1, [r0]            // unlock
    bx lr                   // return
    .endfunc
    
    
#if 0

// this impelementations for __int_compare_and_swap is not 100% atomic because if current thread is preemted in point between 
// the two swp instructions and old *addr is different from old (second swp is executed), new thread will see wrong value 
// in *addr (it will see new and it should see old)
// On arm7tdmi this is the best that can be done without disabling interrupts

// The same is for __spinsem_trydec and __spinsem_tryinc

    
	.global __int_compare_and_swap
	.func __int_compare_and_swap
    // int __int_compare_and_swap(int *addr, int old, int new)
    // returns old *addr value
    // int __bool_compare_and_swap(int *addr, int old, int new) { if(__int_compare_and_swap(addr, old, new) == old) return 1; return 0; }
__int_compare_and_swap:
    // addr in r0, old in r1, new in r2, used registers r3, r4
    stmdb sp!, {r4}         // push clober registers on stack
1:
    ldr r4, [r0]            // r4 = *addr
    mov r3, r4              // r3 = *addr
    cmp r4, r1              // *addr == old
    bne 1f                  // if(*addr != old) return 0; FAILED
    swp r3, r2, [r0]        // r3 = *addr; *addr = new
    cmp r4, r3              // *addr == *addr
    swpne r4, r3, [r0]      // if not successful restore old value
    bne 1b                  // and try again
1:
    mov r0, r3
    ldmia sp!, {r4}         // pop clober registers from stack
    bx lr                   // return
    .endfunc
    
	
	
	.global __int_compare_and_swap_irq
	.func __int_compare_and_swap_irq
    // int __int_compare_and_swap_irq(int *addr, int old, int new)
    // returns old *addr value
    // int __bool_compare_and_swap(int *addr, int old, int new) { if(__int_compare_and_swap(addr, old, new) == old) return 1; return 0; }
    // addr in r0, old in r1, new in r2, used registers r3, r4
__int_compare_and_swap_irq:    
	stmdb sp!, {r4}         // push clober registers on stack
	
	// disable interrupts, save status to r4
	mrs r3,cpsr                   // get PSR
	and r4, r3, #I_BIT | F_BIT    // original mask
   	orr r3, r3, #I_BIT | F_BIT    // disable IRQ, disable FIQ
   	msr cpsr, r3                  // set up status register
	
	// do CAS logic
    ldr r3, [r0]            // r3 = *addr
    cmp r3, r1              // *addr == old
    bne 1f                  // if(*addr != old) return old_value; FAILED
    str r2, [r0]        	// *addr = new
1:
	// we should return old value from addr location
	mov r0, r3
	
	// restore interrupt status
	ldr r1,=(~(I_BIT|F_BIT))// disable IRQ and FIQ
	orr r2,r2,r1
	mrs r1,cpsr				// get PSR
	and r2,r2,r1
	msr cpsr,r2				// set up status register
	
    ldmia sp!, {r4}         // pop clober registers from stack
    bx lr                   // return
    .endfunc


    
    
    
	.global __spinsem_trydec
	.func __spinsem_trydec
    // int __spinsem_trydec(unsigned *addr)
    // returns 0 for success, 1 otherwise
__spinsem_trydec:
    // addr in r0, used registers r1, r2, r3
1:
    ldr r1, [r0]            // r1 = *addr
    cmp r1, #0              // *addr == 0
    beq 1f                  // semaphore is blocked, return 1
    sub r2, r1, #1          // r2 = r1 - 1; decrement semaphore
    swp r3, r2, [r0]        // attempt to store new semaphore value; old value in r3
    cmp r1, r3              // *addr == *addr
    swpne r2, r3, [r0]      // if not successful restore old value
    bne 1b                  // and try again
    mov r0, #0              // success, return 0
    bx lr                   // return
1:
    mov r0, #1              // fail, return 1
    bx lr                   // return
    .endfunc
    

    
	.global __spinsem_tryinc
	.func __spinsem_tryinc
    // int __spinsem_tryinc(unsigned *addr)
    // returns 0 for success, 1 otherwise
__spinsem_tryinc:
    // addr in r0, used registers r1, r2, r3
1:
    ldr r1, [r0]            // r1 = *addr
    add r2, r1, #1          // increment semaphore value
    swp r3, r2, [r0]        // attempt to store new semaphore value; old value in r3
    cmp r1, r3              // *addr == *addr
    swpne r2, r3, [r0]      // if not successful restore old value
    bne 1b                  // and try again
    mov r0, #0              // success, return 0
    bx lr                   // return
    .endfunc
    
    
#endif // if 0

    
