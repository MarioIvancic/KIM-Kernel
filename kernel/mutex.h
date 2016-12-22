// mutex.h
// POSIX-like mutex implementation

#ifndef __MUTEX_H__
#define __MUTEX_H__

#include "kernel_arch.h"
#include "kernel.h"
#include "klist.h"

/*
    Description
    Mutual exclusion locks (mutexes) prevent multiple tasks from simultaneously executing
    critical sections of code which access shared data (i.e., mutexes are used to serialize
    the execution of tasks). All mutexes must be global. A successful call for a mutex
    lock via mutex_lock() or mutex_timedock() will cause another task that is also trying
    to lock the same mutex to block until the owner task unlocks it via mutex_unlock().
    
    Initialize
    A mutex must be initialized to unlocked state with a call to mutex_init() before it can be used.
    
    Lock and Unlock
    A critical section of code is enclosed by a the call to lock the mutex and the call to
    unlock the mutex to protect it from simultaneous access by multiple taska. Only one
    task at a time may possess mutually exclusive access to the critical section of code
    that is enclosed by the mutex-locking call and the mutex-unlocking call to protect the
    shared data. A task calling to lock the mutex either gets exclusive access to the code
    starting from the successful locking until its call to unlock the mutex, or it waits
    until the mutex is unlocked by the task that locked it.

    Mutexes have ownership, unlike semaphores. Although any task, within the scope of a mutex,
    can get an unlocked mutex and lock access to the same critical section of code, only the
    task that locked a mutex can unlock it.

    A call to mutex_lock() locks the mutex. If the mutex is already locked, the calling task
    blocks until the mutex is freed; this will return with the mutex object in the locked 
    state with the calling task as its owner. If the current owner of a mutex tries to relock
    the mutex, it will result in deadlock.

    mutex_trylock() is the same as mutex_lock() except that if the mutex is locked (by any task,
    including the current task), the call returns immediately with an error.
    
    mutex_unlock() is called by the owner of the mutex object referenced by mp to release it.
    The mutex must be locked and the calling task must be the one that last locked the mutex
    (the owner). If there are tasks blocked on the mutex when mutex_unlock() is called, the 
    scheduling policy will determine which task gets the mutex. If the calling task is not the
    owner of the mutex, no error status is returned, and the behavior of the program is undefined.
    
    If successful, all of these functions return 0 ; otherwise, -1 is returned.
    
    In this implementation priority ceiling is used to prevent priority inversion.
*/


typedef struct mutex
{
    uatomic_t   value;
    klist_t     pend_queue;
    TCB_t*      owner;
} mutex_t;


// Intialize mutex to unlocked state.
// This must be the first operation on mutex.
atomic_t mutex_init(mutex_t *mp);


// Try to lock a mutex. If already locked returns -1.
// If successful it will boost tasks priority if needed and return 0
// but it will not switch context and it will not block
atomic_t mutex_trylock(mutex_t *mp );


// Mutext unlock function.
// If calling task is not the owner of the mutex return value is -1 (error).
// If mutex is already unlocked 0 is returned without blocking and context switching.
// If mutex was locked before this call it will be unlocked and original task priority
// will be restored. If there is blocked tasks in mutex pending queue first task will
// be unblocked and it will become new mutex owner. Unblocked task will have boosted
// priority so it will run before this function returns.
// Return value is 0 (success).
atomic_t mutex_unlock(mutex_t *mp );


// Try to lock a mutex. If successful it will boost tasks priority
// if needed and return 0. If mutex is already locked it will block
// for ticks kernel ticks or until mutex is sucessfuly locked.
// If mutex is successfuly locked return value is 0. In case a timeout
// occurred return value is -1.
atomic_t mutex_timedlock(mutex_t *mp, unsigned int ticks);


// Mutext lock function. If successful it will boost tasks priority
// if needed and return 0. If mutex is already locked it will block
// until mutex is sucessfuly locked, so it might wait forever.
// If mutex is successfuly locked return value is 0.
static inline atomic_t mutex_lock(mutex_t *mp) { return mutex_timedlock(mp, 0); }


#endif // __MUTEX_H__
