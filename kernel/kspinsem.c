// kspinsem.c
// lightweight semaphore implementation (with OS support).

#include "kspinsem.h"
#include "kernel.h"

#ifndef KERNEL_HAVE_ATOMIC_SPINSEM
// some MCUs don't have support for atomic spinsem_trydec and spinsem_tryinc

#include "kspinlock.h"

// decrement semaphore if not blocked
// returns 0 for success, 1 otherwise
atomic_t __spinsem_trydec(spinsem_t * sem)
{
	// try to accuire semaphore lock, return 1 if not seccessful
	if(kspinlock_trylock(&sem->lock)) return 1;
	
	// semaphore spinlock is locked
	// if sem->value is 0 semaphore is blocked, return 1
	if(sem->value == 0)
	{
		kspinlock_unlock(&sem->lock);
		return 1;
	}
	
	// semaphore value is not 0, decrement it and return 0 (success)
	sem->value -= 1;
	kspinlock_unlock(&sem->lock);
	
	return 0;
}


// increment semaphore
// returns 0 for success, 1 otherwise
atomic_t __spinsem_tryinc(spinsem_t *sem)
{
	// try to accuire semaphore lock, return 1 if not seccessful
	if(kspinlock_trylock(&sem->lock)) return 1;
	
	// semaphore spinlock is locked
	// increment it and return 0 (success)
	sem->value += 1;
	kspinlock_unlock(&sem->lock);
	
	return 0;
}

#endif // KERNEL_HAVE_ATOMIC_SPINSEM


// decrement semaphore
// if semaphore is blocked it tries spin_counts times calling task_yield() every time
// return 0 if sucess, 1 if timeout
atomic_t kspinsem_timeddec(spinsem_t *sem, unsigned spin_counts)
{
    do
    {
    	if(! kspinsem_trydec(sem)) return 0;
    	task_yield();
    } while(--spin_counts);
        
    return 1;
}


// decrement semaphore, wait forever calling task_yield() if blocked
void kspinsem_dec(spinsem_t *sem)
{
    while(kspinsem_trydec(sem)) task_yield();
}



// increment semaphore
void kspinsem_inc(spinsem_t *sem)
{
    while(__spinsem_tryinc(sem)) task_yield();
}




