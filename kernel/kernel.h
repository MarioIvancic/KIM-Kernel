// kernel.h

#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <stdint.h>
#include "klist.h"
#include "kernel_arch.h"

/*
KERNEL_MAX_PRIORITIES defines priority levels, from 0 to KERNEL_MAX_PRIORITIES - 1.
Lowest priority level is 0, highest priority level is KERNEL_MAX_PRIORITIES - 1.

Lowest priotity level (level 0) is used for main idle loop that will never block.
If there is other tasks beside of main idle loop on lowest priority level then those
tasks will share CPU time with main idle loop because main idle loop will never block.

Hasks holding a mutex (owners of mutex) will temporarily have maximum priority level
KERNEL_MAX_PRIORITIES - 1.

Taking all this in consideration, it's recomended to reserve priority level 0 and
priority level KERNEL_MAX_PRIORITIES - 1 for this special purposes and for normal
tasks use some priority levels from 1 to KERNEL_MAX_PRIORITIES - 2.

Good default value for 2-4 tasks is 4.
*/

#ifndef KERNEL_MAX_PRIORITIES
#define KERNEL_MAX_PRIORITIES 4
#endif // KERNEL_MAX_PRIORITIES


typedef void*       thread_t;
typedef uint8_t     task_prio_t;
typedef uint8_t     task_state_t;
typedef uint16_t    task_flags_t;
typedef uint16_t    task_quota_t;
typedef uint16_t    task_refc_t;


typedef struct TCB
{
    void *  sp;                 // The task's current stack pointer
    int     stack_size;
    klist_node_t node;          // linked list pointers for run_queue and wait_queue
    klist_node_t semanode;      // linked list pointers for semaphore and mutex
    klist_t* semalist;          // pointer to linked list in semaphore/mutex
    //-----------------
    task_quota_t quota;
    task_quota_t tick_counter;
    //-----------------
    task_flags_t flags;         // Status flags includes activity status, parent task, etc
    task_state_t state;         // process state
    task_prio_t  priority;
    task_prio_t  savedprio;     // for mutex
} TCB_t;



// task flags
typedef enum
{
	TASK_SEMA_TIMEOUT = (1 << 0),
} task_flags_e;



// task state
typedef enum
{
	TASK_READY = 0,
	TASK_BLOCKED,
	TASK_SLEEP,
	TASK_STOPPED,
	TASK_TIMEOUT,
	TASK_DEAD,
	TASK_STATES
} task_state_e;


extern klist_t  kernel_run_queue[];
extern klist_t  kernel_wait_queue;

extern volatile TCB_t* kernel_current_task;
extern volatile atomic_t kernel_running;
extern volatile task_prio_t max_prio;
extern uatomic_t kernel_ticks;


// internal functions
void kernel_context_switcher(void);
void kernel_tick_proc(void);
void kernel_arch_init_task_stack(TCB_t* tcb, void* fcn, void* arg);
atomic_t kernel_arch_init(int ticks_for_interrupt);
atomic_t kernel_arch_start(void);
void kernel_trigger_cs(void);

// helper function for blocking current task on semaphore or mutex.
// used in sem_timedwait() and mutex_timedlock()
void kernel_block_task(klist_t* pend_queue, unsigned ticks);

// helper function for unblocking task on semaphore, mutex or queue
void kernel_unblock_task(klist_t* pend_queue);

///////////////////////////////////////////////////////////////////////////////////

// api callable from main

// function for initialization of kernel framework
// must be called first (before other kernel functions) with interrupts disabled
atomic_t kernel_init(unsigned ticks_for_interrupt);


// This function will start kernel multitasking framework. Interrupts must 
// be disabled and it will be enabled inside this function. It will return
// and code below call to this function will become main idle task.
atomic_t kernel_start(void);


// api callable from main or from task

// enter critical section and disable other tasks to modify task table
void kernel_enter_critical(atomic_t* state);

// exit critical section and enable other tasks to modify task table
void kernel_exit_critical(atomic_t* state);

// exit critical section and enable other tasks to modify task table
// trigger context switch
void kernel_exit_critical_cs(atomic_t* state);


// get current task handle (pointer to TCB_t struct)
TCB_t* task_get_handle(void);


// get current tick count
unsigned kernel_get_ticks(void);

// get elapsed time in kernel ticks
unsigned kernel_get_elapsed_ticks(unsigned start_time);

// Get current high speed tick count. Kernel high speed tick counter is usually
// hardware timer. On some CPUs reading kernel high speed tick counter is not 
// atomic and interrupts may be disabled for a short time.
unsigned kernel_get_hsticks(void);

// get elapsed time in kernel high speed ticks
unsigned kernel_get_elapsed_hsticks(unsigned start_time);


// Create new task with entry point p, argument arg, stack of size stack_size
// on memory block at address stack_addr, priority prio and time quota quota.
// The new task will be in TASK_READY state but it's run time will depend on
// other runable tasks. 
// Caller task is not blocked.
// If quota is > 0 new task will be preempted every quota kernel ticks.
// If quota == 0 new task will not have time quota and will be preempted only
// on task_usleep(), mutex_lock(), sem_wait().
atomic_t task_create(TCB_t* tcb, void* (*p)(void*), void* arg,
                void* stack_addr,
                unsigned stack_size,
                task_prio_t prio,
                task_quota_t quota);


// api callable from task only

// Task should not normally return but if it does it may call
// task_exit before return to explicitlly terminate task. 
// If it just returns without calling task_exit directly this 
// function will be called indirectly becouse task_exit is 
// return point for every task except for main idle loop.
void task_exit(void);


// task_yield will give CPU to some other
// task on the same or higher priority level.
void task_yield(void);


// Kill task of same or lower priority.
// If task == 0 current (calling) task is killed.
// After call to this function all resources of killed task
// can be reused (like memory, etc.)
// Note that main idle loop can't be killed.
void task_kill(volatile TCB_t* task);

// sleep number of ticks
void task_usleep(unsigned ticks);

// Get priority of a task. If task is NULL return priority of current task.
task_prio_t task_get_priority(TCB_t* task);

// Set new task priority to new_prio. If task is NULL set priority of current
// task. Returns old task priority.
task_prio_t task_set_priority(TCB_t* task, task_prio_t new_prio);



#endif // __KERNEL_H__
