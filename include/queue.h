
#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"

#define MAX_QUEUE_SIZE 10
#define MAX_INT 1000000
/* Array-based queue */
struct queue_t
{
    struct pcb_t *proc[MAX_QUEUE_SIZE];
    int size; // Keep track number of processes
};

/**
 * @brief 
 *      Add a new process into the queue in a `sorted` manner. So that we can
 *      retrieve the process with least priority easily.
 * 
 * @note 
 *      Although MLQ algorithm never uses priorities within queue, just implement
 *      it - NK.
 */
void enqueue (struct queue_t *q, struct pcb_t *proc);

/**
 * @brief 
 *      Get the process with the \b least priority number (aka \b highest priority), 
 *      and remove that element from the queue.
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
struct queue_t* init_queue();

/**
 * @brief 
 *      Reclaim memory allocated for queue
 * 
 * @brief
 *      The pcb_t(s) controlled by this queue is NOT automatically destroyed.
 */
void destroy_queue(struct queue_t *);

#endif
