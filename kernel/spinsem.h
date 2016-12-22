// spinsem.h
// lightweight semaphore implementation (with no OS support).

#ifndef __SPINSEM_H__
#define __SPINSEM_H__

// some MCUs don't have support for atomic spinsem_trydec and spinsem_tryinc
#ifdef HAVE_ATOMIC_SPINSEM

typedef unsigned spinsem_t;

static inline void spinsem_init(spinsem_t *sem) { *sem = 0; }
static inline unsigned spinsem_value(spinsem_t *sem) { return *sem; }

#else	// HAVE_ATOMIC_SPINSEM

#include "spinlock.h"

typedef struct spinsem
{
	volatile int lock;
	unsigned value;
} spinsem_t;

static inline void spinsem_init(spinsem_t *sem) { sem->lock = SPINLOCK_UNLOCKED; sem->value = 0; }
static inline unsigned spinsem_value(spinsem_t *sem) { return sem->value; }

#endif // HAVE_ATOMIC_SPINSEM



// decrement semaphore if not blocked
// returns 0 for success, 1 otherwise
extern int __spinsem_trydec(spinsem_t *);

// increment semaphore
// returns 0 for success, 1 otherwise
extern int __spinsem_tryinc(spinsem_t *);
    

// decrement semaphore if not blocked
// returns 0 for success, 1 otherwise
static inline int spinsem_trydec(spinsem_t *sem)
{
    return __spinsem_trydec(sem);
}


// decrement semaphore, wait forever if blocked
static inline void spinsem_dec(spinsem_t *sem)
{
    while(__spinsem_trydec(sem));
}



// increment semaphore
// returns 0 for success, 1 otherwise
static inline int spinsem_tryinc(spinsem_t *sem)
{
    return __spinsem_tryinc(sem);
}



// increment semaphore
// returns old semaphore value
static inline void spinsem_inc(spinsem_t *sem)
{
    while(__spinsem_tryinc(sem));
}


// decrement semaphore
// if semaphore is blocked it tries spin_counts
// return 0 if sucess, 1 if timeout
int spinsem_timeddec(spinsem_t *sem, unsigned spin_counts);


#endif // __SPINSEM_H__

