/**
 * @file sched.c
 * @category Implementation source code
 * @brief
 *      Implementation of Multi-level queue (MLQ), which is a set of queues
 * with different priority.
 *
 *      MLQ has two main operations: get_proc() and put_proc()
 *      get_proc() chooses a process from one of its queues (based on MLQ
 * algorithm) to feed the CPU.
 *
 *      put_proc() push back the residual process back to the queue of same
 * priority.
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
 *      Group discussion required to decide whether we should use ready_queue
 * or not (to make sure that our OS runs fine on both MLQ and single-queue).
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
static int current_prio = 0;

#ifdef MLQ_SCHED
/* Define an array of queues */
static struct queue_t mlq_ready_queue[MAX_PRIO];
#endif

#ifdef MLQ_SCHED
void
reset_slots (void)
{
    unsigned long prio;
    for (prio = 0; prio < MAX_PRIO; prio++)
        mlq_ready_queue[prio].slots = 0; // Reset the round-robin counter
                                         // for each every queue
    current_prio = 0;
}
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
            mlq_ready_queue[i].slots = 0;
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
    pthread_mutex_destroy(&queue_lock);
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
{
    /**TODO:
     Get a process from PRIORITY [ready_queue].
     Use lock to protect the queue.
     @return the process from ready_queue
     */
    struct pcb_t *proc = NULL;

    /* If all queues are empty, return NULL */
    if (queue_empty () == 1)
        return proc;

    int designated_slots;
    int num_queues_out_slots = 0;
    while (1)
        {
            /** LOOP THROUGH MLQ
             * @if a queue is empty
             * continue to next queue
             * @else
             *  @if ran out of designated slots (time slices)?
             *  continue to next queue
             *  @else
             *  get the first process
             */
            // If all queues have run out of slots,
            // we reset their slots and are back
            // to the highest priority queue.
            if (num_queues_out_slots == MAX_PRIO)
                reset_slots ();
            // Otherwise, compute the number of slots a queue gets
            designated_slots = MAX_PRIO - current_prio;
            // access the corresponding prio queue in the MLQ
            struct queue_t *priority_queue = &mlq_ready_queue[current_prio];
            // check emptiness
            if (priority_queue->size != 0)
                {
                    // out of slots?
                    if (priority_queue->slots == designated_slots)
                        {
                            // increase the out-of-slots counter
                            num_queues_out_slots++;
                            // increase the prio counter
                            current_prio = (current_prio + 1) % MAX_PRIO;
                            // on to next queue
                            continue;
                        }
                    else // just get the first process from the queue
                        {
                            proc = dequeue (priority_queue);
                            // because a process will exec in 1 time slice,
                            // increment "slots" counter by 1
                            priority_queue->slots++;
                            return proc;
                        }
                }
            else
                current_prio
                    = (current_prio + 1) % MAX_PRIO; // check next queue
        }
}

void
put_mlq_proc (struct pcb_t *proc)
{
    /** TODO
     * adds a process to the MLQ policy according to its priority
     * @remark maybe, this func is for putting a not finished proc into the
     * queue.
     *
     * @remark NK agreed with your idea
     *
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
     *
     * @remark NK agreed with your idea
     */
    pthread_mutex_lock (&queue_lock);
    enqueue (&mlq_ready_queue[proc->prio], proc);
    pthread_mutex_unlock (&queue_lock);
}

/* Get a proc from queue */
struct pcb_t *
get_proc (void)
{
    return get_mlq_proc ();
}

/* Put an unfinised proc back to queue */
void
put_proc (struct pcb_t *proc)
{
    return put_mlq_proc (proc);
}

/**
 * @brief Add a new proc into queue
 * @attention This method is just to distinguish from putting an unfinished
 * proc back to queue. Two func basically behave the same, but separated to
 * prevent confusion while using the interface.
 */
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
