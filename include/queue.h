
#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"

#define MAX_QUEUE_SIZE 10
#define MAX_INT 1000000
/**
 * @brief Array-based queue. 
 * Three attributes:
 *      proc : a pointer to a stack-allocated contiguous array
 *      size : dynamic size, a counter of processes. New processes
 *              will be added at position size-th.
 *      slots : number of round-robin quantum time spared for a queue.
 * 
 * @note This queue should be initialized and deinitialized with init_queue()
 * and destroy_queue(). Any direct access to the attributes are deprecated.
 * Since it is not safe to have any external code to access into the queue
 * internals.
 * 
 */
struct queue_t
{
    struct pcb_t *proc[MAX_QUEUE_SIZE]; // ptr to contiguous array
    int size; // Number of processes in queue, dynamically changes as the queue
              // grow
    int slots; // Number of time slots used by the queue
};

/**
 * @brief
 *      Add a new process into the queue in a `sorted` manner. So that we can
 *      retrieve the process with least priority easily.
 *
 * @note
 *      Although MLQ algorithm never uses priorities within queue, just
 * implement it - NK.
 */
void enqueue (struct queue_t *q, struct pcb_t *proc);

/**
 * @brief
 *      Get the process with the \b least priority number (aka \b highest
 * priority), and remove that element from the queue.
 *
 * @return A ptr to the dequeued pcb_t
 */
struct pcb_t *dequeue (struct queue_t *q);

/**
 * @brief
 *      Check if the queue is empty or not
 *
 * @return 1 - empty queue, 0 - otherwise.
 */
int empty (struct queue_t *q);

/**
 * @brief
 *      Initialize queue
 */
struct queue_t *init_queue ();

/**
 * @brief
 *      Reclaim memory allocated for queue
 *
 * @brief
 *      The pcb_t(s) controlled by this queue is NOT automatically destroyed.
 */
void destroy_queue (struct queue_t *);

#endif
