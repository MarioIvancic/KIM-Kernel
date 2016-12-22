// kspinsem.h
// lightweight semaphore implementation (with OS support).

/*
    Incrementing a semaphore is non-blocking operation.
    
    Decrementing a semaphore is operation that can block.
    
    If semaphore is already blocked calling task will yield
    in a hope that some other task will increment semaphore.
    
    There is no protection from deadlock so tasks using one
    semaphore MUST HAVE THE SAME priority.
*/



#ifndef __KSPINSEM_H__
#define __KSPINSEM_H__

#include "kernel_arch.h"

// some MCUs don't have support for atomic spinsem_trydec and spinsem_tryinc
#ifdef KERNEL_HAVE_ATOMIC_SPINSEM

typedef uatomic_t spinsem_t;

static inline void kspinsem_init(spinsem_t *sem) { *sem = 0; }
static inline uatomic_t kspinsem_value(spinsem_t *sem) { return *sem; }

#else	// KERNEL_HAVE_ATOMIC_SPINSEM

#include "kspinlock.h"

typedef struct spinsem
{
	atomic_t lock;
	uatomic_t value;
} spinsem_t;

static inline void kspinsem_init(spinsem_t *sem) { sem->lock = KSPINLOCK_UNLOCKED; sem->value = 0; }
static inline uatomic_t kspinsem_value(spinsem_t *sem) { return sem->value; }

#endif // KERNEL_HAVE_ATOMIC_SPINSEM


// decrement semaphore if not blocked
// returns 0 for success, 1 otherwise
atomic_t __spinsem_trydec(spinsem_t *);

// increment semaphore
// returns 0 for success, 1 otherwise
atomic_t __spinsem_tryinc(spinsem_t *);


// decrement semaphore if not blocked
// returns 0 for success, 1 otherwise
static inline atomic_t kspinsem_trydec(spinsem_t *sem)
{
    return __spinsem_trydec(sem);
}


// decrement semaphore, wait forever calling task_yield() if blocked
void kspinsem_dec(spinsem_t *sem);


// increment semaphore
void kspinsem_inc(spinsem_t *sem);


// decrement semaphore
// if semaphore is blocked it tries spin_counts times calling task_yield() every time
// return 0 if sucess, 1 if timeout
atomic_t kspinsem_timeddec(spinsem_t *sem, unsigned spin_counts);


#endif // __KSPINSEM_H__

