
#ifndef QUEUE_H
#define QUEUE_H

#include "common.h"

#define MAX_QUEUE_SIZE 10

struct queue_t
{
    struct pcb_t *proc[MAX_QUEUE_SIZE];
    int size;
};

/**
 * @brief 
 *      Add a new process into the queue.
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

#endif
