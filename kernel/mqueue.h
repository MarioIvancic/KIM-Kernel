// mqueue.h
// POSIX-like message queue implementation

#ifndef __MQUEUE_H__
#define __MQUEUE_H__

//#include "kernel_arch.h"
#include "kernel.h"
#include "klist.h"



// message structure
// message can hold 2 pointers or 2*sizeof(void*) bytes
typedef union
{
    struct
    {
        union
        {
            void*   pa;
            uint8_t sa[sizeof(void*)];
        };
        union
        {
            void*   pb;
            uint8_t sb[sizeof(void*)];
        };
    };
    uint8_t         byte[2 * sizeof(void*)];
} message_t;




// message queue structure
typedef struct mqueue
{
    uatomic_t head;
    uatomic_t tail;
    uatomic_t mask;         // mask = size - 1;
    message_t* q;           // array of message_t objects
    klist_t   rd_pend_queue;
    klist_t   wr_pend_queue;
} mqueue_t;




/*
    mq_init() function will initialize message queue structure mq. Storage for message
	queue is message_t array q of size qsize. qsize argument must be power of 2 or 
	message queue operation are undefined.
    mq_init() always returns 0.
*/
atomic_t mq_init(mqueue_t* mq, message_t* q, int qsize);



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
atomic_t mq_timedreceive(mqueue_t* mq, message_t *msg, unsigned int ticks);



/*
    mq_receive() receives the oldest of messages from the message queue fifo mq.
    If the message queue mq is empty mq_receive() blocks until a message is queued on the
    message queue. If more than one task is waiting to receive a message when a message 
	arrives at an empty queue, the task that has waited the longest receives the message.
    mq_receive() always returns 0.
*/
static inline atomic_t mq_receive(mqueue_t* mq, message_t *msg) { return mq_timedreceive((mq), (msg), 0); }


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
atomic_t mq_timedsend(mqueue_t* mq, const message_t *msg, unsigned int ticks);




/*
    mq_send() adds the message msg to the message queue specified by mq.
    If the specified message queue is not full, mq_send() inserts message
	to fifo head. If the specified message queue is full mq_send() blocks 
	until space becomes available to enqueue the message. If more than one 
	task is waiting to send when space becomes available in the message 
	queue, the task that has waited the longest sends its message.
    mq_send() always returns 0.
*/
static inline atomic_t mq_send(mqueue_t* mq, const message_t *msg) { return mq_timedsend((mq), (msg), 0); }



/*
    mq_tryreceive() receives the oldest of messages from the message queue fifo mq.
    If the message queue mq is empty mq_tryreceive() returns -1.
    mq_timedreceive() returns 0 on success and -1 if message queue is empty.
*/
atomic_t mq_tryreceive(mqueue_t* mq, message_t *msg);



/*
    mq_trysend() adds the message pointed to by msg to the message queue mq.
    If the message queue is not full, mq_trysend() inserts message to fifo head.
    If the message queue is full mq_trysend() returns -1.
    mq_trysend() returns 0 on success and -1 if message queue is full.
*/
atomic_t mq_trysend(mqueue_t* mq, const message_t *msg);

#endif // __MQUEUE_H__
