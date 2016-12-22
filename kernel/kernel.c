// kernel.c

#include "kernel_arch.h"
#include "kernel.h"

// this is for user task stacks
#ifndef KERNEL_STACK_ALIGN
#define KERNEL_STACK_ALIGN 8
#endif // KERNEL_STACK_ALIGN



// priority array, every element is TCB list
klist_t  kernel_run_queue[KERNEL_MAX_PRIORITIES];

// list of tasks that sleeps and tasks that waits on mutex/semaphore
klist_t  kernel_wait_queue;


volatile TCB_t* kernel_current_task;

// max_prio is runtime max priority of all ready tasks
// it's used to automatically select priority level in task table
volatile task_prio_t max_prio;

// flag that indicates kernel is running
volatile atomic_t kernel_running;

// kernel time is derived from kernel_ticks
unsigned kernel_ticks;

// this is TCB for main idle loop.
static TCB_t main_tcb;


// helper function for blocking current task on semaphore or mutex.
// used in sem_timedwait() and mutex_timedlock()
void kernel_block_task(klist_t* pend_queue, unsigned ticks)
{
    // current task should block on semaphore
    // we should move task to wait queue (it ticks > 0)
    // we should insert task to semaphore pend queue
    // we should set task state to TASK_BLOCK
    klist_remove_node(&kernel_run_queue[kernel_current_task->priority], (klist_node_t*)&kernel_current_task->node);

    if(ticks)
    {
        klist_insert_tail(&kernel_wait_queue, (klist_node_t*)&kernel_current_task->node);
    }

    klist_insert_tail(pend_queue, (klist_node_t*)&kernel_current_task->semanode);

    kernel_current_task->flags &= ~TASK_SEMA_TIMEOUT;

    kernel_current_task->tick_counter = ticks;
    kernel_current_task->state = TASK_BLOCKED;
    
    // set pointer to mutex pend_queue
    kernel_current_task->semalist = pend_queue;
}


// helper function for unblocking task on semaphore, mutex or queue
void kernel_unblock_task(klist_t* pend_queue)
{
	if(pend_queue->n)
	{
		// pend queue is not empty, we must unblock one task
		// since new tasks are added to list tail and we want to unblock
		// task that has been waiting longest we have to unblock first task in list
		TCB_t* task = klist_get_instance(pend_queue->head.next, TCB_t, semanode);

		// remove task from message queue pend queue
		klist_remove_node(pend_queue, &task->semanode);

		// remove task from wait queue depending on tick_counter
		// if task tick_counter is not 0, we have to remove task from wait queue
		// -- insert task to correct run queue
		// if task is not in TASK_BLOCKED state we know we have timeout and task is
		// already in TASK_READY state (timeout case is dealt with in mq_timedreceive)
		if(task->tick_counter)
		{
			klist_remove_node(&kernel_wait_queue, &task->node);
		}
		klist_insert_tail(&kernel_run_queue[task->priority], &task->node);

		// we do not need to switch context here
		// we'll just continue execution (we have priority)
		// task that is just unblocked will have to wait it's turn
	}
}



// function for initialization of kernel framework
// must be called first (before other kernel functions) with interrupts disabled
atomic_t kernel_init(unsigned ticks_for_interrupt)
{
    uatomic_t i;

    kernel_current_task = &main_tcb;
    kernel_next_task = 0;
    kernel_do_cs = 0;
    max_prio = KERNEL_MAX_PRIORITIES - 1;
    kernel_running = 0;
    kernel_ticks = 0;

    for(i = 0; i < KERNEL_MAX_PRIORITIES; i++)
    {
        klist_init(&kernel_run_queue[i]);
    }

    klist_init(&kernel_wait_queue);
    
    // create dummy task that will run main idle loop
    // no entry point, no argument, no stack, stack_size = 0, prio = 0, quota = 0
    // task_create(&main_tcb, 0, 0, 0, 0, 0, 0);
    main_tcb.stack_size = 0;
    main_tcb.priority = 0;
    main_tcb.savedprio = 0;
    main_tcb.quota = 0;
    main_tcb.tick_counter = 0;
    main_tcb.state = TASK_READY;
    main_tcb.semalist = 0;
    klist_node_init(&main_tcb.node);
    klist_node_init(&main_tcb.semanode);

    // now, we have to insert main idle loop task in task table
    klist_insert_tail(&kernel_run_queue[0], &main_tcb.node);
    
    return kernel_arch_init(ticks_for_interrupt);
}


// This function will start kernel multitasking framework. Interrupts must 
// be disabled and it will be enabled inside this function. It will return
// and code below call to this function will become main idle task.
atomic_t kernel_start(void)
{
    // we must have idle task
    if(kernel_run_queue[0].n == 0) return -1;

    /*
    // from v0.2 kernel starts in main idle loop so this code is not needed
    // we must set kernel_current_task to point to first highest priority task
    int i;
    for(i = KERNEL_MAX_PRIORITIES - 1; i >= 0; i--)
    {
        if(kernel_run_queue[i].n)
        {
            kernel_current_task =
                klist_get_instance(klist_head(&kernel_run_queue[i]), TCB_t, node);
            break;
        }
    }
    */

    // we have to adjust stack frame
    return kernel_arch_start();
}



// This scheduler is called whenever a context switch needs to take place,
//	which is anytime:
//	- a Semaphore is set
//	- a task manually yields control
//	- an RTOS tick takes place
//	- a task is set to sleep
//	- a task is left to pend on a semaphore.
//
//	The scheduler is priority-based, and is fully pre-emptive.
//	Where there	are multiple tasks sharing the same priority,
//	the scheduler is round-robin, alternating through all *ready*
//	tasks of the same priority group.
//  It will find highest priority ready task to run, or it will run current task
void kernel_context_switcher(void)
{
    uatomic_t i;
    // Iterate through all priority levels from hiest to lowest
    // and find first non-empty task list.
    // If no new task is found run current task.
    // NOTE: it's prefered to have idle task that newer blocks
    // (created with priority 0 and quota 0). Otherwise, it can
    // happen that kernel will run task when it needs to block

    //for(i = KERNEL_MAX_PRIORITIES - 1; i >= 0; i--)
    for(i = max_prio; i >= 0; i--)
    {
        if(kernel_run_queue[i].n)
        {
            // this level is not empty
            // pick the first task in list and run it
            kernel_current_task = klist_get_instance(kernel_run_queue[i].head.next, TCB_t, node);
            
            // set quota for tick counter
            kernel_current_task->tick_counter = kernel_current_task->quota;
            
            // set max_prio to priority of new task
            max_prio = i;
            break;
        }
    }
}



// This function is called at a regular interval (the RTOS Tick interval)
// and is used to update the task time for each task in the system.
//
// For tasks that are blocked on semaphores, if a timeout occurs on a semaphore,
// flag is set in TCB - this condition should be checked by the sem_timedwait()
// and mutex_timedlock() functions to ensure that correct error handling occurs.
//
// Tasks that are sleeping have their sleep time decremented, and are placed
// in the ready state when the timeout expires.
//
void kernel_tick_proc(void)
{
    kernel_ticks++;

    // check quota for current task
    // if quota is 0 it is treated as infinite
    // and task is never preempted because of quota
    if(kernel_current_task->tick_counter)
    {
        kernel_current_task->tick_counter--;
        if(! kernel_current_task->tick_counter)
        {
            // quota is gone, rotate queue left and then run 
            // the scheduler to find next task to run
            klist_rotate_left(&kernel_run_queue[kernel_current_task->priority]);
            kernel_trigger_cs();
        }
    }

    // now iterate through wait_queues and check semaphores/mutexes, sleep timers
    // NOTE: if semaphore/mutex is without timeout task will be in semaphore/mutex
    // pend_queue. In kernel wait_queue we have tasks in state TASK_BLOCKED waiting
    // on semaphore/mutex with timeout, and we have tasks in state TASK_SLEEP waiting
    // on sleep time to expire
    klist_node_t *node;

    klist_iterate_fw(&kernel_wait_queue, node)
    {
        TCB_t* task = klist_get_instance(node, TCB_t, node);

        // it's assumed task->tick_counter is not 0 initially
        task->tick_counter--;

        if(! task->tick_counter)
        {
            // sleep time expired or semaphore/mutex timeout

            // if task->state == TASK_BLOCKED mark timeout in TCB
            // and remove task from semaphore/mutex pend queue
            // NOTE: task is still in semaphore pend queue but
            // task will resume in sem_timedwait() function and
            // in that function task will be removed from semaphore
            // pend queue
            if(task->state == TASK_BLOCKED)
            {
                task->flags |= TASK_SEMA_TIMEOUT;
                
                // we have to remove current task from mutex pend queue
                // because we are not here because of mutex_unlock
                klist_remove_node(task->semalist, &task->semanode);
                task->semalist = 0;
            }

            // set task state to ready,
            // move task to ready queue,
            // if task priority is higher than
            // current task priority run scheduler
            task->state = TASK_READY;

            // we are iterating list and 'node' is current node pointer
            // since we have to move 'node' to another list we have
            // to save pointer to node before 'node'. In next iteration
            // current node will be node after 'node' as if it's not
            // removed from list
            int i = task->priority;
            klist_node_t* t = node->prev;
            klist_remove_node(&kernel_wait_queue, node);
            klist_insert_tail(&kernel_run_queue[i], node);
            node = t;

            if(i > max_prio)
            {
                max_prio = i;
                kernel_trigger_cs();
            }
        }
    }
}


// application API functions

// api callable from main of from task

// Create new task with entry point p, argument arg, stack of size stack_size
// on memory block at address stack_addr, priority prio and time quota quota.
// The new task will be in TASK_READY state but it's run time will depend on
// other runable tasks. 
// Caller task is not blocked.
// If quota is > 0 new task will be preempted every quota kernel ticks.
// If quota == 0 new task will not have time quota and will be preempted only
// on task_usleep(), mutex_lock(), sem_wait().
int task_create(TCB_t* tcb, void* (*p)(void*), void* arg,
                void* stack_addr,
                int stack_size,
                task_prio_t prio,
                task_quota_t quota)
{
    // first some error checking
    if(prio >= KERNEL_MAX_PRIORITIES) return -1;

    // tcb->sp have to point to TopOfStack
    tcb->sp = (void*)(((int32_t)stack_addr + stack_size) & ~(KERNEL_STACK_ALIGN - 1));
    tcb->stack_size = stack_size;
    tcb->priority = prio;
    tcb->savedprio = prio;
    tcb->quota = quota;
    tcb->tick_counter = quota;
    tcb->state = TASK_READY;
    tcb->semalist = 0;
    klist_node_init(&tcb->node);
    klist_node_init(&tcb->semanode);

    kernel_arch_init_task_stack(tcb, p, arg);

    // now, we have to insert task in task table
    // - enter kernel critical section
    atomic_t state;
    kernel_enter_critical(&state);

    klist_insert_tail(&kernel_run_queue[prio], &tcb->node);
    if(prio > max_prio) max_prio = prio;
    // we do not need to call scheduler, will just wait to timer tick
    // or yield from other task to do the thing

    kernel_exit_critical(&state);

    return 0;
}



// sleep number of ticks
void task_usleep(unsigned int ticks)
{
    // Tick count is in ticks and task is always the current task.
    // Also, we know that current task is in TASK_READY state or
    // we would not be here.
    // So, set tick count, change task state, remove it from run_queue
    // and insert it to wait_queue. After all that do context switch.
    atomic_t state;
    kernel_enter_critical(&state);

    kernel_current_task->tick_counter = ticks;
    kernel_current_task->state = TASK_SLEEP;
    klist_remove_node(&kernel_run_queue[kernel_current_task->priority],
                     (klist_node_t*)&kernel_current_task->node);
    klist_insert_tail(&kernel_wait_queue, (klist_node_t*)&kernel_current_task->node);
	
    kernel_exit_critical_cs(&state);
}


// Task should not normally return but if it does it may call
// task_exit before return to explicitlly terminate task. 
// If it just returns without calling task_exit directly this 
// function will be called indirectly becouse task_exit is 
// return point for every task except for main idle loop.
void task_exit(void)
{
    task_kill(0);
}


// Kill task of same or lower priority.
// If task == 0 current (calling) task is killed.
// After call to this function all resources of killed task
// can be reused (like memory, etc.)
// Note that main idle loop can't be killed.
void task_kill(volatile TCB_t* task)
{
    // If task pointer is NULL we should kill current task.
    // task can kill only task on same or lower priority level.
    // when task calls task_kill we have to
    // - remove task from run_queue
    // - set task state to TASK_DEAD
    // - do context switch

    atomic_t state;
    kernel_enter_critical(&state);

    if(!task) task = kernel_current_task;
    if(kernel_current_task->priority >= task->priority)
    {
        klist_remove_node(&kernel_run_queue[task->priority],
                     (klist_node_t*)&task->node);
        task->state = TASK_DEAD;
        
        // remove task from any semaphore/mutex pend_queue
        if(task->semalist)
        {
            klist_remove_node(task->semalist, (klist_node_t *)&task->semanode);
            task->semalist = 0;
        }
    }
	
    kernel_exit_critical_cs(&state);
}


// get current task handle (pointer to TCB_t struct)
TCB_t* task_get_handle(void)
{
    /* while in user task kernel_current_task will always be valid
    so there is no need to use critical section
    volatile TCB_t* task;
    atomic_t state;

    kernel_enter_critical(&state);

    task = kernel_current_task;

    kernel_exit_critical(&state);

    return (TCB_t*)task;
    */
    return (TCB_t *)kernel_current_task;
}


// task_yield will give CPU to some other
// task on the same or higher priority level.
void task_yield(void)
{
    if(kernel_running)
	{
	    atomic_t state;
	    kernel_enter_critical(&state);

	    // current task give CPU to some other task on the same
        // or higher priority queue. So, we have to rotate run_queue
        // and do context switch
        klist_rotate_left(&kernel_run_queue[kernel_current_task->priority]);
		
        kernel_exit_critical_cs(&state);
	}
}


// Get priority of a task. If task is NULL return priority of current task.
task_prio_t task_get_priority(TCB_t* task)
{
    if(!task) task = kernel_current_task;
    return task->priority;
}


// Set new task priority to new_prio. If task is NULL set priority of current
// task. Returns old task priority.
task_prio_t task_set_priority(TCB_t* task, task_prio_t new_prio)
{
    task_prio_t old_prio;
    atomic_t state;
    
    if(!task) task = kernel_current_task;
    
    kernel_enter_critical(&state);
    
    old_prio = task->priority;
    if(new_prio != old_prio)
    {
        klist_remove_node(&kernel_run_queue[old_prio], (klist_node_t*)&task->node);
        klist_insert_tail(&kernel_run_queue[new_prio], (klist_node_t*)&task->node);
    }
    
    kernel_exit_critical_cs(&state);
    
    return old_prio;
}

