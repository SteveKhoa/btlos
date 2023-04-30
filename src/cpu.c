/**
 * @file cpu.c
 * @category Implementation source code
 * @brief
 *      Definitions of four operations: calc, alloc, free_data, read and
 *      a wrapper run() which executes an instruction and returns status
 *      code (0 or 1).
 *
 */
#include "cpu.h"
#include "mem.h"
#include "mm.h"

/**
 *      Always return 0
 *
 *  @return
 *      0 always successful
 */
int
calc (struct pcb_t *proc)
{
    return ((unsigned long)proc & 0UL);
}

/**
 *      Claim `size` bytes in memory, and store the address to register
 * `reg_index`.
 *
 *  @return
 *      1 if not successful,
 *      0 if suceesful
 */
int
alloc (struct pcb_t *proc, uint32_t size, uint32_t reg_index)
{
    addr_t addr = alloc_mem (size, proc);
    if (addr == 0)
        {
            return 1;
        }
    else
        {
            proc->regs[reg_index] = addr;
            return 0;
        }
}

/**
 *      Free the memory at address store in register `reg_index`.
 *
 *
 * @return
 *      0 if address valid
 *      1 if address invalid
 */
int
free_data (struct pcb_t *proc, uint32_t reg_index)
{
    return free_mem (proc->regs[reg_index], proc);
}

/**
 *      Read the memory at addess `source`+`offset`, store the data into
 *      register `destination`.
 *
 * @return
 *      0 if successful,
 *      1 if unsuccessful
 */
int
read (struct pcb_t *proc, // Process executing the instruction
      uint32_t source,    // Index of source register
      uint32_t offset,    // Source address = [source] + [offset]
      uint32_t destination)
{ // Index of destination register

    BYTE data;
    if (read_mem (proc->regs[source] + offset, proc, &data))
        {
            proc->regs[destination] = data;
            return 0;
        }
    else
        {
            return 1;
        }
}

/**
 *      Write `data` to register `destination`+`offset`
 *
 * @return
 *      0 if successful,
 *      1 if unsuccessful
 */
int
write (struct pcb_t *proc,   // Process executing the instruction
       BYTE data,            // Data to be wrttien into memory
       uint32_t destination, // Index of destination register
       uint32_t offset)
{ // Destination address =
    // [destination] + [offset]
    return write_mem (proc->regs[destination] + offset, proc, data);
}

/**
 *      Execute AN instruction in the process and increase the program counter
 *      by one. This func does NOT execute all instructions at once.
 *
 * @return
 *      0 if successful,
 *      1 if unsuccessful or no more instructions
 */
int
run (struct pcb_t *proc)
{
    /* Check if the Program Counter points to the proper instruction */
    if (proc->pc >= proc->code->size)
        {
            return 1;
        }

    struct inst_t ins = proc->code->text[proc->pc];
    proc->pc++; // Increase the program counter
    int stat = 1;
    switch (ins.opcode)
        {
        case CALC:
            stat = calc (proc);
            break;
        case ALLOC:
#ifdef MM_PAGING
            stat = pgalloc (proc, ins.arg_0, ins.arg_1);

#else
            stat = alloc (proc, ins.arg_0, ins.arg_1);
#endif
            break;
        case FREE:
#ifdef MM_PAGING
            stat = pgfree_data (proc, ins.arg_0);
#else
            stat = free_data (proc, ins.arg_0);
#endif
            break;
        case READ:
#ifdef MM_PAGING
            stat = pgread (proc, ins.arg_0, ins.arg_1, ins.arg_2);
#else
            stat = read (proc, ins.arg_0, ins.arg_1, ins.arg_2);
#endif
            break;
        case WRITE:
#ifdef MM_PAGING
            stat = pgwrite (proc, ins.arg_0, ins.arg_1, ins.arg_2);
#else
            stat = write (proc, ins.arg_0, ins.arg_1, ins.arg_2);
#endif
            break;
        default:
            stat = 1;
        }
    return stat;
}
