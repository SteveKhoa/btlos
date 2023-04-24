/**
 * @file sched.c
 * @category Implementation source code
 * @brief
 *      Implementation of Multi-level queue (MLQ), which is a set of queues
 * with different priority.
 *
 *      MLQ has two main operations: get_proc() and put_proc()
 *      get_proc() chooses a process from one of its queues (based on MLQ algorithm)
 *      to feed the CPU.
 *
 *      put_proc() push back the residual process back to the queue of same priority-signature.
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
    /**
     * Check if all queues in MLQ are empty
     * */
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
        {
            mlq_ready_queue[i].size = 0;
            mlq_ready_queue[i].time_slices = 0;
        }
#endif
    // DEPRECATED
    // ready_queue.size = 0;
    // DEPRECATED
    // run_queue.size = 0;
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
 *  prio)
 */
struct pcb_t *
get_mlq_proc (void)
#pragma clang diagnostic push
//#pragma ide diagnostic ignored "UnreachableCode"
{
    /**TODO:
     Get a process from PRIORITY [ready_queue].
     Use lock to protect the queue.
     @return the process from ready_queue
     */
    struct pcb_t *proc = NULL;

    /* If all queues are empty, return NULL */
    if (queue_empty() == 1) return proc;

    int priority = 0;
    int time_slice = 0;
    int designated_slots;
    while (priority < MAX_PRIO){
            /** Loop through MLQ.
             * @if a queue is empty
                * continue to next queue
             * @else
             *  @if ran out of designated time slices?
                *  continue to next queue
             *  @else
                *  get the first process
             */
            designated_slots = MAX_PRIO - priority;
            struct queue_t priority_queue = mlq_ready_queue[priority];
            if (priority_queue.size != 0){
                if (priority_queue.time_slices == designated_slots)
                        {
                            // reset the time slices count:
                            priority_queue.time_slices = 0;
                            // and
                            priority++;
                            continue;
                            // to next queue
                        }
                    else
                    {
                        proc = dequeue (&priority_queue);
                        // because a process will exec in 1 time slice,
                        // increment time slices count by 1
                        priority_queue.time_slices++;
                        break;
                    }
                }

        }
    return proc;
}
//#pragma clang diagnostic pop

void
put_mlq_proc (struct pcb_t *proc)
{
    /** TODO
     * adds a process to the MLQ policy according to its priority
     * @remark maybe, this func is for putting a not finished proc into the queue.
     */
    pthread_mutex_lock (&queue_lock);
    enqueue (&mlq_ready_queue[proc->prio], proc);
    pthread_mutex_unlock (&queue_lock);
}

void
add_mlq_proc (struct pcb_t *proc)
{
    /** TODO
     * @attention What is the difference between this and put_mlq_proc?
     * @remark maybe, this func is for adding a new proc into the queue.
     */
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
