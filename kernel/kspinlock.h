// kspinlock.h
// lightweight lock implementation (with OS support).

#ifndef __KSPINLOCK_H__
#define __KSPINLOCK_H__

#include "kernel_arch.h"

/*
    Unlocking a spinlock is non-blocking operation.
    
    Locking a spinlock is operation that can block.
    
    If spinlock is already locked calling task will yield
    in a hope that some other task will unlock spinlock.
    
    There is no protection from deadlock so tasks using one
    spinlock MUST HAVE THE SAME priority.
*/

#define KSPINLOCK_UNLOCKED 0
#define KSPINLOCK_LOCKED 1


// returns 0 for success, 1 otherwise
extern atomic_t __spinlock_trylock(atomic_t *addr);
extern void __spinlock_unlock(atomic_t *addr);


// Init spinlock to unlocked state
static inline void kspinlock_init(atomic_t *lock)
{
    *lock = KSPINLOCK_UNLOCKED;
}


// nonblocking kspinlock lock function
// return 0 if sucess, 1 if already locked
static inline atomic_t kspinlock_trylock(atomic_t *lock)
{
    return __spinlock_trylock(lock);
}


// unlock spinlock
static inline void kspinlock_unlock(atomic_t* lock)
{
    __spinlock_unlock(lock);
}


// lock spinlock, call task_yield in a loop if already locked
void kspinlock_lock(atomic_t *lock);

// semiblocking kspinlock lock function
// call task_yield in a loop if already locked
// It tries at most spin_count times to lock spinlock
// For spin_count = 0 it tries MAX_UINT times
// return 0 if sucess, 1 if timeout
atomic_t kspinlock_timedlock(atomic_t *lock, unsigned spin_count);


#endif // __KSPINLOCK_H__

