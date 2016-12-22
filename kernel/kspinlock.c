// kspinlock.c
// lightweight lock implementation (with OS support)

#include "kspinlock.h"
#include "kernel.h"

// lock spinlock, call task_yield in a loop if already locked
void kspinlock_lock(atomic_t *lock)
{
     while(__spinlock_trylock(lock)) task_yield();
}


// semiblocking kspinlock lock function
// call task_yield in a loop if already locked
// It tries at most spin_count times to lock spinlock
// For spin_count = 0 it tries MAX_UINT times
// return 0 if sucess, 1 if timeout
atomic_t kspinlock_timedlock(atomic_t *lock, unsigned spin_count)
{
    do
    {
    	if(! __spinlock_trylock(lock)) return 0;
    	task_yield();
    } while(--spin_count);
    
    return 1;
}
