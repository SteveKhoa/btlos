/**
 * @file queue.c
 * @category Implementation source code
 * @brief
 *      Implementation from `queue.h` interface
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
    if (q->size < MAX_QUEUE_SIZE - 1)
        {
            q->proc[q->size] = proc; // Put a proc at the front of the queue
            q->size++;               // Increase the cnt
        }
}

int
queuePeek (struct queue_t *q)
{
    int maxPrio = MAX_INT;
    int index = -1;

    for (int i = 0; i < q->size; i++)
        {
            if (maxPrio > q->proc[i]->priority)
                {
                    maxPrio = q->proc[i]->priority;
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
#ifdef MLQ_SCHED // Skip the priority and dequeue the first proc if using MLQ

    if (!empty (q))
        {
            struct pcb_t *newProc = malloc (sizeof (struct pcb_t));
            newProc = q->proc[0];
            for (int i = 0; i < q->size; i++)
                {
                    q->proc[i] = q->proc[i + 1];
                }
            q->size--;
            return newProc;
        }

#else
    if (!empty (q))
        {
            int index = queuePeek (q);
            struct pcb_t *newProc = malloc (sizeof (struct pcb_t));
            newProc = q->proc[index];
            for (int i = index; i < q->size; i++)
                {
                    q->proc[i] = q->proc[i + 1];
                }
            q->size--;
            return newProc;
        }
#endif
    return NULL;
}

/* Heap-allocate the queue, and initialize its attributes*/
struct queue_t *
init_queue ()
{
    /* The name should be changed to `ptr` to avoid confusion
        of using the global variable */
    struct queue_t *ptr = malloc (sizeof (struct queue_t));
    ptr->size = 0;
    return ptr;
};

/* Reclaim the heap-allocation of the queue */
void
destroy_queue (struct queue_t *q)
{
    q->size = -1; // To signify that this queue is invalid
                  // Later access to this 'garbage value' can be debugged
                  // easier
    free (q);
}
