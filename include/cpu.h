
#ifndef CPU_H
#define CPU_H

#include "common.h"

/* Execute an instruction of a process. Return 0
 * if the instruction is executed successfully.
 * Otherwise, return 1. */
/**
 * @brief Execute an instruction in a process. Increase the program counter by one.
 * @return 
 *      0 if ins executed, and pc increased;
 *      1 if end of file, or error.
*/
int run (struct pcb_t *proc);

#endif
