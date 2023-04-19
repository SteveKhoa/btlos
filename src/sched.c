/**
 * @file sched.c
 * @category Implementation source code
 * @brief
 *      Implementation of Multi-level queue (MLQ), which is a set of queues
 * with different priority.
 *
 *      MLQ has two main operations: get_proc() and put_proc(), get_proc()
 * wisely choose a process from one of its queues (based on MLQ algorithm) to
 * feed the CPU. put_proc() push back the residual process back to the queue of
 * same priority-signature.
 *
 *
 * @note
 *      We distinguish MLQ (a set of queues) and queue (a normal FIFO priority
 * queue). Priority-signature means the queue with its associated priority.
 * (ask group for further clarification).
 *
 *      There are tons of legacy code in this file, just read the specification
 *      carefully and decide which should not be implemented. NK purposefully
 * mark not useful declarations as deprecated. (both as comments and as macros)
 * 
 * @warning
 *      Group discussion required to decide whether we should use ready_queue or
 *      not (to make sure that our OS runs fine on both MLQ and single-queue).
 *
 */
#include "sched.h"
#include "queue.h"
#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
__attribute__ ((deprecated)) static struct queue_t ready_queue; // DEPRECATED
__attribute__ ((deprecated)) static struct queue_t run_queue;   // DEPRECATED
static pthread_mutex_t queue_lock;

#ifdef MLQ_SCHED
/* Define an array of queues */
static struct queue_t mlq_ready_queue[MAX_PRIO];
#endif

int
queue_empty (void)
{
#ifdef MLQ_SCHED
    /* All queues in MLQ must be empty, otherwise */
    unsigned long prio;
    for (prio = 0; prio < MAX_PRIO; prio++)
        if (!empty (&mlq_ready_queue[prio]))
            return -1;
    return 1;
#endif // MLQ_SCHED
    // return (empty (&ready_queue) && empty (&run_queue));
    // DEPRECATED
}

void
init_scheduler (void)
{
#ifdef MLQ_SCHED
    int i;

    for (i = 0; i < MAX_PRIO; i++)
        mlq_ready_queue[i].size = 0;
#endif
    // ready_queue.size = 0;
    // DEPRECATED
    // run_queue.size = 0;
    // DEPRECATED
    pthread_mutex_init (&queue_lock, NULL);
}

void
finish_scheduler (void)
{
    /* TO-DO */
}

#ifdef MLQ_SCHED
/*
 *  Stateful design for routine calling
 *  based on the priority and our MLQ policy
 *  We implement stateful here using transition technique
 *  State representation   prio = 0 .. MAX_PRIO, curr_slot = 0..(MAX_PRIO -
 * prio)
 */
struct pcb_t *
get_mlq_proc (void)
{
    struct pcb_t *proc = NULL;
    /*TODO: get a process from PRIORITY [ready_queue].
     * Remember to use lock to protect the queue.
     * */
    return proc;
}

void
put_mlq_proc (struct pcb_t *proc)
{
    pthread_mutex_lock (&queue_lock);
    enqueue (&mlq_ready_queue[proc->prio], proc);
    pthread_mutex_unlock (&queue_lock);
}

void
add_mlq_proc (struct pcb_t *proc)
{
    pthread_mutex_lock (&queue_lock);
    enqueue (&mlq_ready_queue[proc->prio], proc);
    pthread_mutex_unlock (&queue_lock);
}

struct pcb_t *
get_proc (void)
{
    return get_mlq_proc ();
}

void
put_proc (struct pcb_t *proc)
{
    return put_mlq_proc (proc);
}

void
add_proc (struct pcb_t *proc)
{
    return add_mlq_proc (proc);
}
#else  // if not MLQ_SCHED (DEPRECATED)
struct pcb_t *
get_proc (void)
{
    struct pcb_t *proc = NULL;
    /*TODO: get a process from [ready_queue].
     * Remember to use lock to protect the queue.
     * */
    return proc;
}

void
put_proc (struct pcb_t *proc)
{
    pthread_mutex_lock (&queue_lock);
    enqueue (&run_queue, proc);
    pthread_mutex_unlock (&queue_lock);
}

void
add_proc (struct pcb_t *proc)
{
    pthread_mutex_lock (&queue_lock);
    enqueue (&ready_queue, proc);
    pthread_mutex_unlock (&queue_lock);
}
#endif // MLQ_SCHED
