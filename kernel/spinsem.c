// spinsem.c
// lightweight semaphore implementation (with no OS support).

#include "spinsem.h"


#ifndef HAVE_ATOMIC_SPINSEM
// some MCUs don't have support for atomic spinsem_trydec and spinsem_tryinc

#include "spinlock.h"

// decrement semaphore if not blocked
// returns 0 for success, 1 otherwise
int __spinsem_trydec(spinsem_t * sem)
{
	// try to accuire semaphore lock, return 1 if not seccessful
	if(spinlock_trylock(&sem->lock)) return 1;
	
	// semaphore spinlock is locked
	// if sem->value is 0 semaphore is blocked, return 1
	if(sem->value == 0)
	{
		spinlock_unlock(&sem->lock);
		return 1;
	}
	
	// semaphore value is not 0, decrement it and return 0 (success)
	sem->value -= 1;
	spinlock_unlock(&sem->lock);
	
	return 0;
}


// increment semaphore
// returns 0 for success, 1 otherwise
int __spinsem_tryinc(spinsem_t *)
{
	// try to accuire semaphore lock, return 1 if not seccessful
	if(spinlock_trylock(&sem->lock)) return 1;
	
	// semaphore spinlock is locked
	// increment it and return 0 (success)
	sem->value += 1;
	spinlock_unlock(&sem->lock);
	
	return 0;
}

#endif // HAVE_ATOMIC_SPINSEM



// decrement semaphore
// if semaphore is blocked it tries spin_counts
// return 0 if sucess, 1 if timeout
int spinsem_timeddec(spinsem_t *sem, unsigned spin_counts)
{
    do { if(! __spinsem_trydec(sem)) return 0; } while(--spin_counts);
        
    return 1;
}




