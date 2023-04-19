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
    if(q->size < MAX_QUEUE_SIZE - 1)
    {
        q->proc[q->size] = proc;
        q->size++;
    }
    
}

int
queuePeek(struct queue_t *q)
{
    int maxPrio = MAX_INT;
    int index = -1;

    for(int i = 0; i < q->size; i++) 
    {
        if(maxPrio > q->proc[i]->priority)
        {
            maxPrio =  q->proc[i]->priority;
            index = i;
        }
    }

    return index;
}

struct pcb_t *
dequeue (struct queue_t *q)
{
    /* TODO: return a pcb whose priority is the highest
     * in the queue [q] and remember to remove it from q
     * */
    if(!empty(q))
    {
        int index = queuePeek(q);
        struct pcb_t *newProc = malloc(sizeof(struct pcb_t));
        newProc = q->proc[index];
        for(int i = index; i < q->size; i++)
        {
            q->proc[i] = q->proc[i+1];
        }
        q->size--;
        return newProc;
    }
    


    return NULL;
}

struct queue_t * init_queue()
{
    /* data */
    struct queue_t *ready_queue = malloc(sizeof(struct queue_t));
    ready_queue->size = 0;
    return ready_queue;
    
};

void destroy_queue(struct queue_t *q)
{
    free(q->proc);
    q->size = 0;
}
