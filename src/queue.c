/**
 * @file queue.c
 * @category Implementation source code
 * @brief 
 *      Implementation from `queue.h` interface
 * 
 * 
 */
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

int
empty (struct queue_t *q)
{
    if (q == NULL)
        return 1;
    return (q->size == 0);
}

void
enqueue (struct queue_t *q, struct pcb_t *proc)
{
    /* TODO: put a new process to queue [q] */
}

struct pcb_t *
dequeue (struct queue_t *q)
{
    /* TODO: return a pcb whose prioprity is the highest
     * in the queue [q] and remember to remove it from q
     * */
    return NULL;
}

struct queue_t* init_queue()
{
    return NULL; // TO-DO, remember to init all ptrs to NULL, and set size = 0
}

void destroy_queue()
{
    ; // TO-DO
}
