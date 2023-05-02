/**
 * @file mem.h
 * @category Interface file
 * @brief User-interface for memory.
*/
#ifndef MEM_H
#define MEM_H

#include "common.h"

#define RAM_SIZE (1 << ADDRESS_SIZE)

/**
 * @brief Initialize parameters and clear the memory to 0.
*/
void init_mem (void);

/**
 * @brief Allocate memory and return its Virtual Adress
 * @param size number of bytes
 * @param proc the process requesting the allocation
 * @return
 *      addr_t (unsigned int) as virtual address if success;
 *      0 if failed to allocate.
*/
addr_t alloc_mem (uint32_t size, struct pcb_t *proc);

/**
 * @brief Free a memory block having the first byte at [address] used by
 * process [proc]. Return 0 if [address] is valid. Otherwise, return 1
 * @attention DEPRECATED
*/
int free_mem (addr_t address, struct pcb_t *proc);

/**
 * @brief Make [data] points to the BYTE at address [address] in Memory.
 * @param address the address to be read
 * @param proc the process requesting the read
 * @param data the pointer to BYTE (char) data.
 * @return 
 *      0 if valid address;
 *      1 if invalid address
*/
int read_mem (addr_t address, struct pcb_t *proc, BYTE *data);

/* Write [data] to 1 byte on the memory pointed by [address] of process
 * [proc]. If given [address] is valid, return 0. Otherwise, return 1 */
/**
 * @brief Write [data] at [address] in Memory.
 * @param address the address to be written
 * @param proc the process requesting to write
 * @param data BYTE (char) data.
*/
int write_mem (addr_t address, struct pcb_t *proc, BYTE data);

/**
 * @brief Dump out the status of all pages, including:
 *  1. chunk of memory the page occupy,
 *  2. PID using the page,
 *  3. its next page.
*/
void dump (void);

#endif
