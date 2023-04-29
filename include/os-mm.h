#ifndef OSMM_H
#define OSMM_H

#define MM_PAGING
#define PAGING_MAX_MMSWP 4 /* max number of supported swapped space */
#define PAGING_MAX_SYMTBL_SZ 30

typedef char BYTE;
typedef uint32_t addr_t;
// typedef unsigned int uint32_t;

struct pgn_t
{
    int pgn;
    struct pgn_t *pg_next;
};

/**
 * @brief
 *          Linked-list based. Control the data 
 *          of a dynamically allocated variable.
 * 
 *          See Figure: 3 for more details
*/
struct vm_rg_struct
{
    unsigned long rg_start;
    unsigned long rg_end;

    struct vm_rg_struct *rg_next;
};

/**
 * @brief
 *          Linked-list based. Area can contain two subset structures: 
 *          free regions (freerg), memory region (vm_rg). See Figure: 3 for more 
 *          details
*/
struct vm_area_struct
{
    unsigned long vm_id;
    unsigned long vm_start;
    unsigned long vm_end;

    unsigned long sbrk;
    /*
     * Derived field
     * unsigned long vm_limit = vm_end - vm_start
     */
    struct mm_struct *vm_mm;
    struct vm_rg_struct *vm_freerg_list;
    struct vm_area_struct *vm_next;
};

/*
 * Memory management struct
 */
struct mm_struct
{
    /**
     * // no docs here currently
    */
    uint32_t *pgd;
    // page directory

    /**
     * Head ptr of the Linked-list of Memory Areas.
    */
    struct vm_area_struct *mmap;

    /**
     * Symbol region table.
     * Currently we support a fixed number of symbol.
    */
    struct vm_rg_struct symrgtbl[PAGING_MAX_SYMTBL_SZ];

    /* list of free page */
    struct pgn_t *fifo_pgn; // Should be deprecated
};

/*
 * FRAME/MEM PHY struct
 */
struct framephy_struct
{
    int fpn;
    struct framephy_struct *fp_next;

    /* Resereed for tracking allocated framed */
    struct mm_struct *owner;
};

struct memphy_struct
{
    /* Basic field of data and size */
    BYTE *storage;
    int maxsz;

    /* Sequential device fields */
    int rdmflg;
    int cursor;

    /* Management structure */
    struct framephy_struct *free_fp_list;
    struct framephy_struct *used_fp_list;
};

#endif
