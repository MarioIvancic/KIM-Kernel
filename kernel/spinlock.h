// spinlock.h
// lightweight lock implementation (with no OS support).

#ifndef __SPINLOCK_H__
#define __SPINLOCK_H__

#define SPINLOCK_UNLOCKED 0
#define SPINLOCK_LOCKED 1

// returns 0 for success, 1 otherwise
extern int __spinlock_trylock(int *addr);
extern void __spinlock_unlock(int *addr);


// nonblocking spinlock lock function
// return 0 if sucess, 1 if already locked
static inline int spinlock_trylock(int *lock)
{
    return __spinlock_trylock(lock);
}

// lock spinlock, block forever if already locked
static inline void spinlock_lock(int *lock)
{
     while(__spinlock_trylock(lock));
}


// unlock spinlock
static inline void spinlock_unlock(int* lock)
{
    __spinlock_unlock(lock);
}



// semiblocking spinlock lock function
// It tries at most spin_count times to lock spinlock
// For spin_count = 0 it tries MAX_UINT times
// return 0 if sucess, 1 if timeout
int spinlock_timedlock(int *lock, unsigned spin_count);


#endif // __SPINLOCK_H__

