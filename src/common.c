#include "common.h"
#include <stdlib.h>

struct pcb_t *
create_pcb (uint32_t pid, uint32_t priority, struct code_seg_t *code,
            uint32_t pc, struct page_table_t *page_table, uint32_t bp)
{
    /*
    This code was copied from loader.c, for a simple initialization of pcb_t
    */
    struct pcb_t *retpcb = (struct pcb_t *)malloc (sizeof (struct pcb_t));
    retpcb->priority = priority;
    retpcb->pid = pid;
    retpcb->code = code;
    retpcb->page_table = (struct page_table_t *)malloc (sizeof (struct page_table_t));
    retpcb->bp = bp;
    retpcb->pc = 0;
    retpcb->prio = -1;

    return retpcb;
}

void
destroy_pcb (struct pcb_t *ptr)
{
    free (ptr->page_table);
    free (ptr);
}