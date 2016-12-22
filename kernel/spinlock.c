// spinlock.c
// lightweight lock implementation (with no OS support).

#include "spinlock.h"


// semiblocking spinlock lock function
// It tries at most spin_count times to lock spinlock
// For spin_count = 0 it tries MAX_UINT times
// return 0 if sucess, 1 if timeout
int spinlock_timedlock(int *lock, unsigned spin_count)
{
    do { if(! __spinlock_trylock(lock)) return 0; } while(--spin_count);
    
    return 1;
}




