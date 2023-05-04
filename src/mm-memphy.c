/**
 * @file mm-memphy.c
 * @category Implementation source code
 * @brief Implementation of physical memory operations. Every memphy_struct is
 * a physical storage device. Note that we use this instead of _ram in mem.c &
 * mem.h because they are legacy and irrelevant to our project.
 */

// #ifdef MM_PAGING
/*
 * PAGING based Memory Management
 * Memory physical module mm/mm-memphy.c
 */

#include "mm.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Increase the [cursor] of [mp] to [offset].
 * @param mp target memphy structure
 * @param offset value
 * @return 0 if successful, -1 if invalid offset
 */
int
MEMPHY_mv_csr (struct memphy_struct *mp, int offset)
{
    if (mp == NULL)
        return -1;

    // if (offset < 0 || offset >= mp->maxsz)
    //     return -1;
    // We dont check for offset here because any value,
    // exceeding the maxsz will stop sequential traversal.
    // That is one of the advantage of sequential over random,
    // it never goes out of range.

    int numstep = 0;

    mp->cursor = 0;
    while (numstep < offset && numstep < mp->maxsz)
        {
            /* Traverse sequentially */
            mp->cursor = (mp->cursor + 1) % mp->maxsz;
            numstep++;
        }

    return 0;
}

/**
 * @brief Let [value] point to the BYTE at address [addr], using sequential
 * increment (move cursor), to simulate sequential read.
 * @param mp target memphy structure
 * @param addr address
 * @param value ptr to BYTE (char) value at [addr]
 * @return 0 if successful, -1 if error
 */
int
MEMPHY_seq_read (struct memphy_struct *mp, int addr, BYTE *value)
{
    if (mp == NULL)
        return -1;

    // if (addr < 0 || addr >= mp->maxsz)
    //     return -1;

    if (mp->rdmflg)
        return -1; /* Not compatible mode for sequential read */

    MEMPHY_mv_csr (mp, addr);
    *value = (BYTE)mp->storage[addr];

    return 0;
}

/**
 * @brief Let [value] point to the BYTE at address [addr], using direct access,
 * to simulate Random access.
 * @param mp target memphy structure
 * @param addr address
 * @param value ptr to BYTE (char) value at [addr]
 * @return 0 if successful, -1 if error
 */
int
MEMPHY_read (struct memphy_struct *mp, int addr, BYTE *value)
{
    if (mp == NULL)
        return -1;

    if (mp->rdmflg)
        {
            if (addr < 0 || addr >= mp->maxsz)
                return -1;
            *value = mp->storage[addr];
        }
    else /* Sequential access device */
        return MEMPHY_seq_read (mp, addr, value);

    return 0;
}

/**
 * @brief Write [value] to [mp] at address [addr], using sequential cursor
 * move.
 * @param mp
 * @param addr
 * @param value
 * @return 0 if succesful, -1 if error.
 */
int
MEMPHY_seq_write (struct memphy_struct *mp, int addr, BYTE value)
{

    if (mp == NULL)
        return -1;

    // if (addr < 0 || addr >= mp->maxsz)
    //     return -1;
    // We don't check for address here because it is sequential traversal

    if (mp->rdmflg)
        return -1; /* Not compatible mode for sequential read */

    MEMPHY_mv_csr (mp, addr);
    mp->storage[addr] = value;

    return 0;
}

/**
 * @brief Write [value] to [mp] at address [addr], using direct mem access.
 * @param mp
 * @param addr
 * @param value
 * @return 0 if succesful, -1 if error.
 */
int
MEMPHY_write (struct memphy_struct *mp, int addr, BYTE data)
{
    if (mp == NULL)
        return -1;

    if (addr < 0 || addr >= mp->maxsz)
        return -1;

    if (mp->rdmflg)
        {
            if (addr < 0 || addr >= mp->maxsz)
                return -1;
            mp->storage[addr] = data;
        }
    else /* Sequential access device */
        return MEMPHY_seq_write (mp, addr, data);

    return 0;
}

/*
 *  MEMPHY_format-format MEMPHY device
 *  @mp: memphy struct
 */
/**
 * @brief Initialized a linked list of free frames for free_fp_list. The size
 * of this linked list is [pagesz].
 * @param mp target memphy structure
 * @param pagesz the size of each frame
 * @return 0 if successful, -1 if error
 */
int
MEMPHY_format (struct memphy_struct *mp, int pagesz)
{
    /* This setting come with fixed constant PAGESZ */
    int numfp = mp->maxsz / pagesz;
    struct framephy_struct *newfst, *fst;
    int iter = 0;

    if (numfp <= 0)
        return -1;

    /* Just routine to generate a linked list */
    /* Init head of free framephy list */
    fst = malloc (sizeof (struct framephy_struct));
    fst->fpn = iter;
    mp->free_fp_list = fst;

    /* We have list with first element, fill in the rest num-1 element member*/
    for (iter = 1; iter < numfp; iter++)
        {
            newfst = malloc (sizeof (struct framephy_struct));
            newfst->fpn = iter;
            newfst->fp_next = NULL;
            fst->fp_next = newfst;
            fst = newfst;
        }

    return 0;
}

int
MEMPHY_get_freefp (struct memphy_struct *mp, int *retfpn)
{
    struct framephy_struct *fp = mp->free_fp_list;

    if (fp == NULL)
        return -1;

    *retfpn = fp->fpn;
    mp->free_fp_list = fp->fp_next;

    /* MEMPHY is iteratively used up until its exhausted
     * No garbage collector acting then it not been released
     */
    free (fp);

    return 0;
}

int
MEMPHY_put_freefp (struct memphy_struct *mp, int fpn)
{
    struct framephy_struct *fp = mp->free_fp_list;
    struct framephy_struct *newnode = malloc (sizeof (struct framephy_struct));

    /* Create new node with value fpn */
    newnode->fpn = fpn;
    newnode->fp_next = fp;
    mp->free_fp_list = newnode;

    return 0;
}

int
MEMPHY_dump (struct memphy_struct *mp)
{
    /*TODO dump memphy contnt mp->storage
     *     for tracing the memory content
     */

    return 0;
}

/*
 *
 */
/**
 * @brief Initialize Physical Memory Structure (Device). Can be used to
 * simulate RAM or SWP device.
 * @param mp NULL ptr
 * @param max_size maximum size of the phymem structure (bytes)
 * @param randomflg 1 if randomly accessed, 0 if sequentially
 * accessed
 */
int
init_memphy (struct memphy_struct *mp, int max_size, int randomflg)
{
    mp->storage = (BYTE *)malloc (max_size * sizeof (BYTE));
    mp->maxsz = max_size;

    MEMPHY_format (mp, PAGING_PAGESZ); // Config free_fp_listx
    mp->used_fp_list = NULL;           // Config used_fp_list

    mp->rdmflg = (randomflg != 0) ? 1 : 0;

    if (!mp->rdmflg) /* Not Ramdom acess device, then it serial device*/
        mp->cursor = 0;

    return 0;
}

// #endif
