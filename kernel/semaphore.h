// semaphore.h
// POSIX-like semaphore impementation

#ifndef __SEMAPHORE_H__
#define __SEMAPHORE_H__

// #include "kernel_arch.h"
#include "kernel.h"
#include "klist.h"

// semaphore
typedef struct sem
{
    uatomic_t value;
    uatomic_t maxval;
    klist_t   pend_queue;
} sem_t;



/*
    sem_init() is used to initialize semaphore sem to value.
    Semaphore limit is set to maxval.
    This semaphore may be used in subsequent calls to sem_wait(),
    sem_trywait(), sem_timedwait() and sem_post(). 
    In this implementation parameter pshared is not used.
    sem_init() always returns 0.
*/
atomic_t sem_init(sem_t *sem, atomic_t pshared /* not used*/, uatomic_t value, uatomic_t maxval);


static inline atomic_t sem_init_binary(sem_t *sem, atomic_t psh /* not used*/, uatomic_t val) { return sem_init(sem, psh, val, 1); }




/*
    sem_trywait() is a function by which a calling task tries to lock a semaphore. 
    A calling task will not be blocked regardless of the value of the semaphore 
    it accesses. If at the time of a call to sem_trywait(), the value of sem is 
    positive, sem_trywait() decrement the value of the semaphore and returns 0.
    If the semaphore's value is 0 sem_trywait() returns -1;
*/
atomic_t sem_trywait(sem_t *sem);


/*
    If at the time of a call to sem_timedwait() the value of sem is positive, sem_timedwait()
    decrement the value of the semaphore, return immediately, and allow the calling process to continue.
    If the semaphore's value is 0 sem_timedwait() will block the calling task until specified 
    timeout expires or some other task increment semaphore value using sem_post().
    The timeout expires when the relative time specified by ticks passes, as measured by the kernel tick
    clock on which timeouts are based. If parameter ticks is 0 timeout time is infinite and sem_timedwait()
    will block untill some task calls sem_port() on this semaphore.
    sem_timedwait() return 0 if the calling process successfully performed the semaphore lock operation
    on the semaphore designated by sem. If timeout expires the sem_timedwait() returns -1.
*/
atomic_t sem_timedwait(sem_t *sem, unsigned int ticks);


static inline atomic_t sem_wait(sem_t *sem) { return sem_timedwait(sem, 0); }



/*
    If, prior to the call to sem_post(), the value of sem was 0, and other tasks were blocked
    waiting for the semaphore, then the task which has been waiting the longest is unblocked.
    If, prior to the call to sem_post(), no other tasks were blocked for the semaphore, then its
    value is incremented by one but value is not allowed to be higher than limit maxval.
    sem_post() always returns 0.
*/
atomic_t sem_post(sem_t *sem );


/*
    sem_getvalue() reads the value of the semaphore sem without affecting the
    state of the semaphore. The value represents an actual semaphore value that
    occurred at some unspecified time during the call to sem_getvalue(), but 
    may not be the actual value of the semaphore when sem_getvalue() is 
    returned to the caller. The value set in sval may be zero or positive. 
    If sval is zero, there may be other tasks waiting for the semaphore; 
    if sval is positive, no one is waiting.
    sem_getvalue() always returns 0
*/
atomic_t sem_getvalue(sem_t *sem, atomic_t *sval);

#endif // __SEMAPHORE_H__
