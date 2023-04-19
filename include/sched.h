#ifndef SCHED_H
#define SCHED_H

#include "common.h"

#ifndef MLQ_SCHED
#define MLQ_SCHED
#endif

#define MAX_PRIO 139

/**
 * @brief
 *      Check whether the MLQ is empty of not.
 *
 * @return
 *      1 on empty, 0 DEPRECATED, -1 on non-empty.
 */
int queue_empty (void);

/* Initialize MLQ scheduler */
void init_scheduler (void);

/* Free the allocated resources by che scheduler */
void finish_scheduler (void);

/**
 * @brief
 *      Get the process from MLQ, and delete that process from MLQ.
 * 
 * @return A ptr to `pcb_t`
 */
struct pcb_t *get_proc (void);

/**
 * @brief 
 *      Put a process back to the MLQ.
 * 
 * @note
 *      The original 'Put a process back to run queue'-documentation is outdated.
 */
void put_proc (struct pcb_t *proc);

/**
 * @brief 
 *      Put a process back to the MLQ. This is an alias for put_proc().
 * 
 * @note
 *      The original 'Put a process back to run queue'-documentation is outdated.
 */
void add_proc (struct pcb_t *proc);

#endif // SCHED_H
