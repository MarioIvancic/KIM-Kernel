// semaphore.c
// POSIX-like semaphore implementation

#include "semaphore.h"
#include "kernel.h"


/*
    sem_init() is used to initialize semaphore sem to value.
    Semaphore limit is set to maxval.
    This semaphore may be used in subsequent calls to sem_wait(),
    sem_trywait(), sem_timedwait() and sem_post(). 
    In this implementation parameter pshared is not used.
    sem_init() always returns 0.
*/
atomic_t sem_init(sem_t *sem, atomic_t pshared /* not used*/, uatomic_t value, uatomic_t maxval)
{
    (void) pshared;

    klist_init(&sem->pend_queue);
    sem->value = value;
    sem->maxval = maxval;
    return 0;
}


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
atomic_t sem_timedwait(sem_t *sem, unsigned int ticks)
{
    atomic_t state;
    atomic_t retval = 0;

    kernel_enter_critical(&state);

    if(sem->value)
    {
        // semaphore is free, take it
        sem->value -= 1;

        kernel_exit_critical(&state);
    }
    else
    {
        // current task should block on semaphore
        // we should move task to wait queue (it ticks > 0)
        // we should insert task to semaphore pend queue
        // we should set task state to TASK_BLOCK
        kernel_block_task(&sem->pend_queue, ticks);
        
        // current task will block now so we trigger context switch
        kernel_exit_critical_cs(&state);
        
//----------------------------------------------------

        kernel_enter_critical(&state);

        if(kernel_current_task->flags & TASK_SEMA_TIMEOUT)
        {
            retval = -1; // timeout
            kernel_current_task->flags &= ~TASK_SEMA_TIMEOUT;
            
            // since it's timeout case we do not have to switch context
        }

        // if it's not timeout task is removed from semaphore
        // pend queue in sem_post() function

        kernel_exit_critical(&state);
    }

    return retval;
}



/*
    sem_trywait() is a function by which a calling task tries to lock a semaphore. 
    A calling task will not be blocked regardless of the value of the semaphore 
    it accesses. If at the time of a call to sem_trywait(), the value of sem is 
    positive, sem_trywait() decrement the value of the semaphore and returns 0.
    If the semaphore's value is 0 sem_trywait() returns -1;
*/
atomic_t sem_trywait(sem_t *sem)
{
    atomic_t state;
    atomic_t retval = 0;

    kernel_enter_critical(&state);

    if(sem->value)
    {
        // semaphore is free, take it
        sem->value -= 1;
    }
    else retval = -1;

    kernel_exit_critical(&state);

    return retval;
}



/*
    If, prior to the call to sem_post(), the value of sem was 0, and other tasks were blocked
    waiting for the semaphore, then the task which has been waiting the longest is unblocked.
    If, prior to the call to sem_post(), no other tasks were blocked for the semaphore, then its
    value is incremented by one but value is not allowed to be higher than limit maxval.
    sem_post() always returns 0.
*/
atomic_t sem_post(sem_t *sem )
{
    atomic_t state;

    kernel_enter_critical(&state);

    if(sem->pend_queue.n)
    {
        // pend queue is not empty, we must unblock one task.
        // Since new tasks are added to list tail and we want to unblock
        // task that has been waiting longest we have to unblock first task in list.
        TCB_t* task = klist_get_instance(sem->pend_queue.head.next, TCB_t, semanode);

        // remove task from semaphore pend queue
        klist_remove_node(&sem->pend_queue, sem->pend_queue.head.next);
        task->semalist = 0;

        // remove task from wait queue depending on tick_counter
        // If task tick_counter is not 0, we have to remove task from wait queue
        // -- insert task to correct run queue
        // If task is not in TASK_BLOCKED state we know we have timeout and task is
        // already in TASK_READY state (timeout case is dealt with in sem_timedwait).
        if(task->tick_counter)
        {
            klist_remove_node(&kernel_wait_queue, &task->node);
        }
        klist_insert_tail(&kernel_run_queue[task->priority], &task->node);

        kernel_exit_critical_cs(&state);
    }
    else
    {
        // there is no blocked tasks
        // increment sem->value
        if(sem->value < sem->maxval) sem->value += 1;

        kernel_exit_critical(&state);
    }

    return 0;
}



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
atomic_t sem_getvalue(sem_t *sem, atomic_t *sval)
{
    atomic_t state;

    kernel_enter_critical(&state);

    *sval = sem->value;

    kernel_exit_critical(&state);

    return 0;
}
