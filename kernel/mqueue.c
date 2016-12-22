// mqueue.c
// POSIX-like message queue implementation

#include "mqueue.h"


/*
    mq_init() function will initialize message queue structure mq. Storage for message
	queue is message_t array q of size qsize. qsize argument must be power of 2 or 
	message queue operation are undefined.
    mq_init() always returns 0.
*/
atomic_t mq_init(mqueue_t* mq, message_t* q, int qsize)
{
    mq->head = 0;
    mq->tail = 0;
    mq->mask = qsize - 1;
    mq->q = q;
    klist_init(&mq->rd_pend_queue);
    klist_init(&mq->wr_pend_queue);

    return 0;
}




/*
    mq_timedreceive() receives the oldest of messages from the message queue fifo mq.
    If the message queue mq is empty mq_timedreceive() blocks until a message is queued on the
    message queue or until the time interval specified in timeout has elapsed. If more than one
    task is waiting to receive a message when a message arrives at an empty queue, the task
    that has waited the longest receives the message.
    If parameter ticks is 0 timeout time is infinite and mq_timedreceive() will block untill
    some task insert some message in this message queue.
    mq_timedreceive() returns 0 on success and -1 on timeout.
*/
atomic_t mq_timedreceive(mqueue_t* mq, message_t *msg, unsigned int ticks)
{
    atomic_t state;
    atomic_t retval = 0;

    kernel_enter_critical(&state);

	// check if FIFO has data
	if(mq->head == mq->tail)    // message queue is empty
	{
		// current task should block on message queue
        // we should move task to wait queue or block queue
        // we should insert task to message queue pend queue
        // we should set task state to TASK_BLOCK
		kernel_block_task(&mq->rd_pend_queue, ticks);

        kernel_exit_critical_cs(&state);
        
//----------------------------------------------------

        kernel_enter_critical(&state);

        if(kernel_current_task->flags & TASK_SEMA_TIMEOUT)
        {
            retval = -1; // timeout
            kernel_current_task->flags &= ~TASK_SEMA_TIMEOUT;

            // since it's timeout case we do not have to switch context
        }
        else
        {
            // if it's not timeout task is removed from
            // message queue pend queue in mq_send() function
            // we now just have to grab the message
            *msg = mq->q[mq->tail];
            mq->tail = (mq->tail + 1) & mq->mask;
        }
	}
	else    // message queue is not empty, grab the message
	{
	    *msg = mq->q[mq->tail];
	    mq->tail = (mq->tail + 1) & mq->mask;

	    // we should check does some tack waits for space in queue
	    // and if it does we should unblock oldest task (first in a pend queue)
	    kernel_unblock_task(&mq->wr_pend_queue);
	}
	
	kernel_exit_critical(&state);

    return retval;
}




/*
    mq_timedsend() adds the message msg to the message queue specified by mq.
    If the specified message queue is not full, mq_timedsend() inserts message
	to fifo head. If the specified message queue is full mq_timedsend() blocks 
	until space becomes available to enqueue the message or until the time 
	interval specified in timeout has elapsed. If more than one task is waiting 
	to send when space becomes available in the message queue, the task that 
	has waited the longest sends its message.
    If parameter ticks is 0 timeout time is infinite and mq_timedsend() will 
	block untill space becomes available in the message queue.
    mq_timedsend() returns 0 on success and -1 on timeout.
*/
atomic_t mq_timedsend(mqueue_t* mq, const message_t *msg, unsigned int ticks)
{
    atomic_t state;
    atomic_t retval = 0;

    kernel_enter_critical(&state);

	// check if there is some space in message queue
	if(((mq->head + 1) & mq->mask) == mq->tail)
	{
	    // no spece left
        // current task should block on message queue
        // we should move task to wait queue
        // we should insert task to message queue pend queue
        // we should set task state to TASK_BLOCK
		kernel_block_task(&mq->wr_pend_queue, ticks);

        kernel_exit_critical_cs(&state);
        
//----------------------------------------------------

        kernel_enter_critical(&state);

        if(kernel_current_task->flags & TASK_SEMA_TIMEOUT)
        {
            retval = -1; // timeout
            kernel_current_task->flags &= ~TASK_SEMA_TIMEOUT;

            // since it's timeout case we do not have to switch context
        }
        else
        {
            // if it's not timeout task is removed from
            // message queue pend queue in mq_receive() function
            // we now just have to send the message
            mq->q[mq->head] = *msg;
            mq->head = (mq->head + 1) & mq->mask;
        }
	}
	else    // message queue is not full, send the message
	{
	    mq->q[mq->head] = *msg;
        mq->head = (mq->head + 1) & mq->mask;

	    // we should check does some task waits on message in queue
	    // and if it does we should unblock oldest task (first in a pend queue)
		kernel_unblock_task(&mq->rd_pend_queue);
	}
	
	kernel_exit_critical(&state);

    return retval;
}



/*
    mq_tryreceive() receives the oldest of messages from the message queue fifo mq.
    If the message queue mq is empty mq_tryreceive() returns -1.
    mq_timedreceive() returns 0 on success and -1 if message queue is empty.
*/
atomic_t mq_tryreceive(mqueue_t* mq, message_t *msg)
{
    atomic_t state;
    atomic_t retval = 0;

    kernel_enter_critical(&state);

	// check if FIFO has data
	if(mq->head == mq->tail)    // message queue is empty
	{
		retval = -1;
	}
	else    // message queue is not empty, grab the message
	{
	    *msg = mq->q[mq->tail];
	    mq->tail = (mq->tail + 1) & mq->mask;

	    // we should check does some tack waits for space in queue
	    // and if it does we should unblock oldest task (first in a pend queue)
	    kernel_unblock_task(&mq->wr_pend_queue);
	}

	kernel_exit_critical(&state);

    return retval;
}




/*
    mq_trysend() adds the message pointed to by msg to the message queue mq.
    If the message queue is not full, mq_trysend() inserts message to fifo head.
    If the message queue is full mq_trysend() returns -1.
    mq_trysend() returns 0 on success and -1 if message queue is full.
*/
atomic_t mq_trysend(mqueue_t* mq, const message_t *msg)
{
    atomic_t state;
    atomic_t retval = 0;

    kernel_enter_critical(&state);

	// check if there is some space in message queue
	if(((mq->head + 1) & mq->mask) == mq->tail)
	{
	    // no spece left
        retval = -1;
	}
	else    // message queue is not full, send the message
	{
	    mq->q[mq->head] = *msg;
        mq->head = (mq->head + 1) & mq->mask;

	    // we should check does some task waits on message in queue
	    // and if it does we should unblock oldest task (first in a pend queue)
	    kernel_unblock_task(&mq->rd_pend_queue);
	}

	kernel_exit_critical(&state);

    return retval;
}

