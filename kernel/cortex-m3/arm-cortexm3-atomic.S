    // atomic operations for arm-cortex-m3

.text

	.global __spinlock_trylock
	.func __spinlock_trylock
    // int __spinlock_trylock(int *addr)
    // returns 0 for success, 1 otherwise
__spinlock_trylock:
    // addr in r0, used registers r1, r2, r3
    mov r2, #1
1:
    ldrex r1, [r0]          // r1 = *addr
    cmp r1, r2              // *addr == 1
    beq 1f                  // already locked, return 1
    strex r1, r2, [r0]      // not locked, attempt to lock it
    cmp r1, r2              // test strex result
    beq 1b                  // strex failed, retry
    dmb                     // data memory barier required before accessing protected resource
    mov r0, #0              // successful, return 0
    bx lr
1:
    mov r0, #1            	// if not successful return 1
    bx lr                   // return
    .endfunc

    
    
    
	.global __spinlock_unlock
	.func __spinlock_unlock
    // void __spinlock_unlock(int *addr)
__spinlock_unlock:
    // addr in r0, used registers r1
    mov r1, #0              // load UNLOCKED value
    dmb                     // data memory barier required before releasing protected resource
    str r1, [r0]            // unlock
    bx lr                   // return
    .endfunc



    
    
	.global __spinsem_trydec
	.func __spinsem_trydec
    // int __spinsem_trydec(unsigned *addr)
    // returns 0 for success, 1 otherwise
__spinsem_trydec:
    // addr in r0, used registers r1, r2, r3
1:
    ldrex r1, [r0]          // r1 = *addr
    cmp r1, #0              // *addr == 0
    beq 1f                  // semaphore is blocked, return 1
    sub r1, #1              // decrement semaphore
    strex r2, r1, [r0]      // attempt to store new semaphore value
    cmp r2, #1              // test strex result
    beq 1b                  // strex failed, retry
    dmb                     // data memory barier required before accessing protected resource
    mov r0, #0              // successful, return 0
    bx lr
1:
    mov r0, #1	            // if not successful return 1
    bx lr                   // return
    .endfunc
    
    
    
	.global __spinsem_tryinc
	.func __spinsem_tryinc
    // int __spinsem_tryinc(unsigned *addr)
    // returns 0 for success, 1 otherwise
__spinsem_tryinc:
    // addr in r0, used registers r1, r2, r3
1:
    ldrex r1, [r0]          // r1 = *addr
    add r1, #1              // increment semaphore value
    strex r2, r1, [r0]      // attempt to store new semaphore value
    cmp r2, #1              // test strex result
    beq 1b                  // strex failed, retry
    dmb                     // data memory barier required before accessing protected resource
    mov r0, #0              // successful, return 0
    bx lr
    .endfunc
    
    
    

	.global __int_compare_and_swap
	.func __int_compare_and_swap
    // int __int_compare_and_swap(int *addr, int old, int new)
    // returns old *addr value
    // int __bool_compare_and_swap(int *addr, int old, int new) { if(__int_compare_and_swap(addr, old, new) == old) return 1; return 0; }
__int_compare_and_swap:
    // addr in r0, old in r1, new in r2, used registers r3, r4
    push {r4}               // push clober registers on stack
1:
    ldrex r3, [r0]          // r3 = *addr
    cmp r3, r1              // *addr == old
    bne 1f                  // if(*addr != old) return 0; FAILED
    strex r4, r2, [r0]      // *addr = new; r4 = strex result
    cmp r4, #1              // test strex result
    beq 1b                  // strex failed, retry
    dmb                     // data memory barier required before accessing protected resource
1:
    mov r0, r3
    pop {r4}                // pop clober registers from stack
    bx lr                   // return
    .endfunc
    
    
    
