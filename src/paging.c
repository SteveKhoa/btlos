/**
 * @file paging.c
 * @category Executable file
 * @brief 
 *      Checking operation of paging (Memory).
 */
#include "cpu.h"
#include "loader.h"
#include "mem.h"
#include <stdio.h>

int
main ()
{
    struct pcb_t *ld = load ("input/p0");
    struct pcb_t *proc = load ("input/p0");
    unsigned int i;
    for (i = 0; i < proc->code->size; i++)
        {
            run (proc);
            run (ld);
        }
    dump ();
    return 0;
}
