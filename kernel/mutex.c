// mutex.c
// POSIX-like mutex implementation

#include "mutex.h"
#include "kernel.h"

#define MUTEX_LOCKED 0
#define MUTEX_UNLOCKED 1


// Intialize mutex to unlocked state.
// This must be the first operation on mutex.
atomic_t mutex_init(mutex_t *mp)
{
    klist_init(&mp->pend_queue);
    mp->value = MUTEX_UNLOCKED;
    mp->owner = 0;
    return 0;
}



// internal function to set priority of mutex owner task to maximum, if needed
// return 1 if task priority is changed, 0 otherwise
static atomic_t bump_priority2(TCB_t* task, mutex_t* mp)
{
    atomic_t prio;

    // save task priority and set owner
    task->savedprio = prio = task->priority;
    mp->owner = (TCB_t*)task;

    if(prio != KERNEL_MAX_PRIORITIES - 1)
    {
        // set new task prioriti to maximum priority
        task->priority = max_prio = KERNEL_MAX_PRIORITIES - 1;

        // move task to correct priority list
        if(klist_node_used((klist_node_t*)&task->node))
        {
            klist_remove_node(&kernel_run_queue[prio], (klist_node_t*)&task->node);
        }
        klist_insert_tail(&kernel_run_queue[max_prio], (klist_node_t*)&task->node);

        // task priority is changed
        return 1;
    }

    // task priority is NOT changed
    return 0;
}

static inline atomic_t bump_priority(mutex_t* mp) { return bump_priority2((TCB_t*)kernel_current_task, mp); }



// Try to lock a mutex. If already locked returns -1.
// If successful it will boost tasks priority if needed and return 0
// but it will not switch context and it will not block
atomic_t mutex_trylock(mutex_t *mp )
{
    atomic_t state;
    atomic_t retval = 0;

    kernel_enter_critical(&state);

    if(mp->value == MUTEX_UNLOCKED)
    {
        // mutex is not locked, lock it
        mp->value = MUTEX_LOCKED;
        
        // timeout is not used
        kernel_current_task->tick_counter = 0;

        // test if we have to boost priority of current task?
        // we don't have to switch context because we own the mutex
        bump_priority(mp);
    }
    else if(kernel_current_task != mp->owner)
    {
        // NOTE: this code is entered with task normal priority

        // mutex is already locked by some other task, current task should
        // get -1 from this function indicating that mutex is not locked
        retval = -1;
    }
    
    // mutex is already locked by THIS task, return value is 0
    // indicating that mutex is successfuly locked

    kernel_exit_critical(&state);

    return retval;
}



// Try to lock a mutex. If successful it will boost tasks priority
// if needed and return 0. If mutex is already locked it will block
// for ticks kernel ticks or until mutex is sucessfuly locked.
// If mutex is successfuly locked return value is 0. In case a timeout
// occurred return value is -1.
atomic_t mutex_timedlock(mutex_t *mp, unsigned int ticks)
{
    atomic_t state;
    atomic_t retval = 0;

    kernel_enter_critical(&state);

    if(mp->value == MUTEX_UNLOCKED)
    {
        // mutex is not locked, lock it
        mp->value = MUTEX_LOCKED;

        // test if we have to boost priority of current task?
        // we don't have to switch context because we own the mutex
        bump_priority(mp);
        kernel_exit_critical(&state);
        
        // return success
        return 0;
    }
    else if(kernel_current_task == mp->owner)
    {
        // mutex is already locked by THIS task, return value is 0
        // indicating that mutex is successfuly locked
        kernel_exit_critical(&state);
        return 0;
    }
    
    // NOTE: this code is entered with task normal priority

    // mutex is already locked by some other task, current task should block on mutex
    // we should move task to kernel wait queue if ticks > 0
    // we should insert task to mutex pend queue
    // we should set task state to TASK_BLOCKED
    kernel_block_task(&mp->pend_queue, ticks);
    
    // current task will block now so we trigger context switch
    kernel_exit_critical_cs(&state);
    
    // --------------------------------------------------------------------------

    // if we got here we have normal priority and
    // some task released mutex or mutex_timedlock() timedout
    
    kernel_enter_critical(&state);

    if(kernel_current_task->flags & TASK_SEMA_TIMEOUT)
    {
        retval = -1; // timeout
        kernel_current_task->flags &= ~TASK_SEMA_TIMEOUT;

        // since it's timeout case we do not have to switch context
    }
    
    // it's not timeout so some task released mutex and it's ours now
    // we don't have to switch context because we own the mutex
    
    kernel_exit_critical(&state);
    return retval;
}




// Mutext unlock function.
// If calling task is not the owner of the mutex return value is -1 (error).
// If mutex is already unlocked 0 is returned without blocking and context switching.
// If mutex was locked before this call it will be unlocked and original task priority
// will be restored. If there is blocked tasks in mutex pending queue first task will
// be unblocked and it will become new mutex owner. Unblocked task will have boosted
// priority so it will run before this function returns.
// Return value is 0 (success).
atomic_t mutex_unlock(mutex_t *mp )
{
    atomic_t state;
    atomic_t do_cs = 0;
    atomic_t prio, savedprio;

    kernel_enter_critical(&state);

    // first test is the current task owner of the mutex?
    if(kernel_current_task != mp->owner)
    {
        kernel_exit_critical(&state);
        return -1;
    }
    
    // if already unlocked return 0 (success)
    if(mp->value == MUTEX_UNLOCKED)
    {
        kernel_exit_critical(&state);
        return 0;
    }

    // current task is releasing this mutex so we have to restore
    // original priority and if needed do a context switch
    prio        = kernel_current_task->priority;
    savedprio   = kernel_current_task->savedprio;

    if(prio != savedprio)
    {
        kernel_current_task->priority = savedprio;

        // move task to correct priority list
        klist_remove_node(&kernel_run_queue[prio], (klist_node_t*)&kernel_current_task->node);
        klist_insert_tail(&kernel_run_queue[savedprio], (klist_node_t*)&kernel_current_task->node);

        do_cs = 1;
    }

    // now we have to see is there any blocked tasks, and if they are we have to 
    // unblock first task in pend queue (task that has been waiting longest)
    if(! mp->pend_queue.n)
    {
        // there is no blocked tasks, unlock mutex
        mp->value = MUTEX_UNLOCKED;
        mp->owner = 0;
    }
    else
    {
        // pend queue is not empty, we must unblock one task but mutex stays locked
        // and unblocked task will become new mutex owner
        
        // since new tasks are added to list tail and we want to unblock the task
        // that has been waiting longest we have to unblock the first task in list
        TCB_t* task = klist_get_instance(mp->pend_queue.head.next, TCB_t, semanode);

        // remove first task from mutex pend queue
        klist_remove_node(&mp->pend_queue, &task->semanode);
        task->semalist = 0;

        // remove task from wait queue depending on tick_counter
        // if task tick_counter is not 0, we have to remove task from wait queue
        // -- bump task priority and insert it to correct run queue
        // -- set task as new mutex owner
        // if task is not in TASK_BLOCKED state we know we had timeout and task is
        // already in TASK_READY state (timeout case is dealt with in mutex_timedlock)
        if(task->tick_counter)
        {
            klist_remove_node(&kernel_wait_queue, &task->node);
        }
        // klist_insert_tail(&kernel_run_queue[task->priority], &task->node);
        bump_priority2(task, mp);
        
        do_cs = 1;
    }

    if(do_cs)   kernel_exit_critical_cs(&state);
    else        kernel_exit_critical(&state);

    return 0;
}
