/**
 * @file mm-memphy.c
 * @category Implementation source code
 * @brief
 *      Implementation of virtual memory operations.
 */

// #ifdef MM_PAGING
/*
 * PAGING based Memory Management
 * Virtual memory module mm/mm-vm.c
 */

#include "mm.h"
#include "string.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Add new region to freerg_list
 * @param mm The list of areas to add rg to
 * @param rg_elmt The rg to be added
 */
int
enlist_vm_freerg_list (struct mm_struct *mm, struct vm_rg_struct rg_elmt)
{
    struct vm_rg_struct *rg_node = mm->mmap->vm_freerg_list;

    if (rg_elmt.rg_start >= rg_elmt.rg_end)
        return -1; // return -1 when the region is invalid

    if (rg_node != NULL)
        rg_elmt.rg_next = rg_node;

    /* Enlist the new region */
    mm->mmap->vm_freerg_list = &rg_elmt;

    return 0;
}

/**
 * @brief Get memory area by `vmaid`.
 * @param mm The list of areas to get memory area from
 * @param vmaid The id of memory area
 */
struct vm_area_struct *
get_vma_by_num (struct mm_struct *mm, int vmaid)
{
    struct vm_area_struct *pvma = mm->mmap;

    if (mm->mmap == NULL)
        return NULL;

    int vmait = 0;

    while (vmait < vmaid)
        {
            if (pvma == NULL)
                return NULL;

            pvma = pvma->vm_next;
        }

    return pvma;
}

/**
 * @brief Get memory region through Symbol table
 * @param mm The list of areas to get the rg from
 * @param rgid The name of symbol
 */
struct vm_rg_struct *
get_symrg_byid (struct mm_struct *mm, int rgid)
{
    if (rgid < 0 || rgid > PAGING_MAX_SYMTBL_SZ)
        return NULL;

    return &mm->symrgtbl[rgid];
}

/**
 * @brief Allocate a new symbol on Virtual Memory.
 * @param caller The process requesting the allocation.
 * @param vmaid The id of memory area to be allocated.
 * @param rgid The name of symbol (currently just index)
 * @param size The size of allocation
 * @param alloc_addr rg_start of the allocation
 */
int
__alloc (struct pcb_t *caller, int vmaid, int rgid, int size, int *alloc_addr)
{
    /*Allocate at the toproof */
    struct vm_rg_struct rgnode;

    // check to see if we can find a region. 0 means yes
    if (get_free_vmrg_area (caller, vmaid, size, &rgnode) == 0)
        {
            caller->mm->symrgtbl[rgid].rg_start = rgnode.rg_start;
            caller->mm->symrgtbl[rgid].rg_end = rgnode.rg_end;

            *alloc_addr = rgnode.rg_start;

            return 0;
        }

    /* When get_free_vmrg_area FAILED to handle the region management,
    we increase limit to get space */
    struct vm_area_struct *cur_vma = get_vma_by_num (caller->mm, vmaid);
    int inc_sz = PAGING_PAGE_ALIGNSZ (size);
    // int inc_limit_ret
    int old_sbrk;

    old_sbrk = cur_vma->sbrk;

    /* INCREASE THE LIMIT */
    inc_vma_limit (caller, vmaid, inc_sz);

    // move sbrk up:
    cur_vma->sbrk += size;

    /*Successfully increase limit */
    caller->mm->symrgtbl[rgid].rg_start = old_sbrk;
    caller->mm->symrgtbl[rgid].rg_end = old_sbrk + size;

    *alloc_addr = old_sbrk;

    return 0;
}

/**
 * @brief Remove a symbol from Virtual memory
 * @param caller: The process requesting the removal.
 * @param vmaid: The id of memory area
 * @param rgid: The name of symbol (currently just numbers allowed)
 */
int
__free (struct pcb_t *caller, int vmaid, int rgid)
{
    // struct vm_rg_struct rgnode;

    if (rgid < 0 || rgid > PAGING_MAX_SYMTBL_SZ)
        return -1;

    struct vm_rg_struct *currg = get_symrg_byid (caller->mm, rgid);

    struct vm_area_struct *cur_vma = get_vma_by_num (caller->mm, vmaid);

    if (currg == NULL || cur_vma == NULL) /* Invalid memory identify */
        return -1;

    // int addr = currg->rg_start;

    /*enlist the obsoleted memory region */
    enlist_vm_freerg_list (caller->mm, *currg);

    return 0;
}

/**
 * @brief
 * @param caller The process requesting the page alloc
 * @param size
 * @param reg_index
 */
int
pgalloc (struct pcb_t *proc, uint32_t size, uint32_t reg_index)
{
    int addr;

    /* By default using vmaid = 0 */
    return __alloc (proc, 0, reg_index, size, &addr);
}

/**
 * @brief Paging-based memory free. Adding the region associated with
 * [reg_index] to free_rg_list
 * @param proc the process requesting
 * @param reg_index rgid
 */
int
pgfree_data (struct pcb_t *proc, uint32_t reg_index)
{
    return __free (proc, 0, reg_index);
}

/**
 * @brief Mappping the page number [pgn] to physical frame number [fpn]. This
 * function tries to get the frame number [fpn], by initializing a page entry,
 * swapping from MEMSWP, or replacing pages if needed.
 * @return 0 if succesful and fpn is updated; -1 if paging failed.
 */
int
pg_getpage (struct mm_struct *mm, int pgn, int *fpn, struct pcb_t *caller)
{
    if (pgn < 0 || pgn >= PAGING_MAX_PGN) // check if pgn is invalid
        {
            return -1;
        }

    uint32_t pte = mm->pgd[pgn];

    if (!PAGING_PAGE_PRESENT (pte)) // if PAGE NOT PRESENT
                                    // pte not initialized
        { /* Page is not online, make it actively living */
            // int vicpgn, swpfpn;
            // int vicfpn;
            // uint32_t vicpte;

            int tgtfpn
                = PAGING_SWP (pte); // the target frame storing our variable

            /* TODO: Play with your paging theory here */

            // ==> My idea
            //  First, we must find if there are free frames or not.
            //      If there are free frames --> get the frame, who needs swp?
            //      If not free frames --> Find victim page --> swap physical
            //      contents of [vicpte] out to SWP, and use that physical
            //      contents as the new frame for [pte].

            int freefpn;
            int get_freefp_status = MEMPHY_get_freefp (caller->mram, &freefpn);

            if (get_freefp_status != -1)
                {
                    pte_set_fpn (&pte, freefpn);
                    printf ("Get free frame from RAM succesfully.\n");
                }
            else
                {
                    int vicpgn;
                    if (find_victim_page (caller->mm, &vicpgn) == -1)
                        {
                            printf ("Get find victim page failed.\n");
                            return -1;
                        }

                    uint32_t vicpte = mm->pgd[vicpgn];

                    // Swap the contents of the current page out

                    int mswp_free_frame; // Find a free frame in active mswp
                    if (MEMPHY_get_freefp (caller->active_mswp,
                                           &mswp_free_frame)
                        == -1)
                        {
                            // Can not get a free frame in the current mswp
                            // Question? Will we skip, or we must implement 4
                            // mswp mechanism

                            // Currently don't know, so assume only one mswp
                            // was used

                            // Answered: we only use one MSWP, if its full,
                            // return error.

                            return -1; // If current mswp full, return error.
                        }
                    int srcfpn_out = PAGING_FPN (vicpte);
                    int dstfpn_out = mswp_free_frame;
                    __swap_cp_page (caller->mram, srcfpn_out,
                                    caller->active_mswp, dstfpn_out);

                    // And swap the contents from SWP of the needed page in.

                    int srcfpn_in = PAGING_SWP (pte);
                    int dstfpn_in = PAGING_FPN (vicpte);
                    __swap_cp_page (caller->mram, srcfpn_in,
                                    caller->active_mswp, dstfpn_in);

                    int phyaddr_swp
                        = (mswp_free_frame << PAGING_ADDR_FPN_LOBIT);
                    int swptyp = 0; // In this assignment, we assume swptyp = 0
                    int swpoff = phyaddr_swp; // SWP OFFSET, where the frame
                                              // actually is on MSWP

                    // PTE off pte and victim pte must be updated

                    pte_set_fpn (&pte, dstfpn_in); // the page now become
                                                   // "RAM"-oriented (32-bits)
                    pte_set_swap (&vicpte, swptyp,
                                  swpoff); // the page now become
                                           // "SWP"-oriented (only 25bits)

                    SETBIT (pte,                      // Make pte "present"
                            PAGING_PTE_PRESENT_MASK); // Duplicate with
                                                      // pte_set_fpn's macro.
                                                      // However, this macro is
                                                      // still put here to
                                                      // clarify the flow of
                                                      // pg_getpage algorithm.
                    CLRBIT (
                        vicpte,
                        PAGING_PTE_PRESENT_MASK); // Make vicpte "unpresent"
                                                  // Note that this CLRBIT must
                                                  // come AFTER pte_set_swap()

                    mm->pgd[vicpgn] = vicpte; // Update page table
                    mm->pgd[pgn] = pte;       // Update page table

                    printf ("Swapped sucessfully, frame %d updated.\n",
                            dstfpn_in);
                }

            /* Do swap frame from MEMRAM to MEMSWP and vice versa*/
            /* Copy victim frame to swap */
            //__swap_cp_page();
            /* Copy target frame from swap to mem */
            //__swap_cp_page();

            /* Update page table */
            // pte_set_swap() &mm->pgd;

            /* Update its online status of the target page */
            // pte_set_fpn() & mm->pgd[pgn];
            // pte_set_fpn (&pte, tgtfpn);

            enlist_pgn_node (&caller->mm->lru_pgn, pgn);
        }

    *fpn = PAGING_FPN (pte);

    return 0;
}

/*pg_getval - read value at given offset
 *@mm: memory region
 *@addr: virtual address to acess
 *@value: value
 *
 */
int
pg_getval (struct mm_struct *mm, int addr, BYTE *data, struct pcb_t *caller)
{
    int pgn = PAGING_PGN (addr);
    int off = PAGING_OFFST (addr);
    int fpn;

    /* Get the page to MEMRAM, swap from MEMSWAP if needed */
    if (pg_getpage (mm, pgn, &fpn, caller) != 0)
        return -1; /* invalid page access */

    int phyaddr = (fpn << PAGING_ADDR_FPN_LOBIT) + off;

    MEMPHY_read (caller->mram, phyaddr, data);

    return 0;
}

/**
 * @brief Request the physical memory frame associated with address [addr] to
 * update its value to [value]. The physical memory will be modified after this
 * function call.
 * @return 0 if successful; -1 if paging failed.
 */
int
pg_setval (struct mm_struct *mm, int addr, BYTE value, struct pcb_t *caller)
{
    int pgn = PAGING_PGN (addr);
    int off = PAGING_OFFST (addr);
    int fpn;

    /* Get the page to MEMRAM, swap from MEMSWAP if needed */
    if (pg_getpage (mm, pgn, &fpn, caller) != 0)
        return -1; /* invalid page access */

    int phyaddr = (fpn << PAGING_ADDR_FPN_LOBIT) + off;

    MEMPHY_write (caller->mram, phyaddr, value);

    return 0;
}

/*__read - read value in region memory
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@offset: offset to acess in memory region
 *@rgid: memory region ID (used to identify variable in symbol table)
 *@size: allocated size
 *
 */
int
__read (struct pcb_t *caller, int vmaid, int rgid, int offset, BYTE *data)
{
    struct vm_rg_struct *currg = get_symrg_byid (caller->mm, rgid);

    struct vm_area_struct *cur_vma = get_vma_by_num (caller->mm, vmaid);

    if (currg == NULL || cur_vma == NULL) /* Invalid memory identify */
        return -1;

    pg_getval (caller->mm, currg->rg_start + offset, data, caller);

    return 0;
}

/*pgwrite - PAGING-based read a region memory */
/**
 * @brief Paging-based memory read.
 * @param proc the process requesting the write
 * @param data BYTE (char) to be written
 * @param destination region id
 * @param offset offset of Virtual Memory Area.
 * @attention the core routine is pg_getval()
 */
int
pgread (struct pcb_t *proc, // Process executing the instruction
        uint32_t source,    // Index of source register
        uint32_t offset,    // Source address = [source] + [offset]
        uint32_t destination)
{
    BYTE data;
    int val = __read (proc, 0, source, offset, &data);

    // destination = (uint32_t)data; // ignore this
    proc->regs[destination] = data;
#ifdef IODUMP
    printf ("read region=%d offset=%d value=%d\n", source, offset, data);
#ifdef PAGETBL_DUMP
    print_pgtbl (proc, 0, -1); // print max TBL
#endif
    MEMPHY_dump (proc->mram);
#endif

    return val;
}

/**
 * @brief Ensure the validity of [rgid] and issue the associated page to update
 * its frame's value.
 * @note Explicit use of this function is DEPRECATED.
 */
int
__write (struct pcb_t *caller, int vmaid, int rgid, int offset, BYTE value)
{
    struct vm_rg_struct *currg = get_symrg_byid (caller->mm, rgid);

    struct vm_area_struct *cur_vma = get_vma_by_num (caller->mm, vmaid);

    if (currg == NULL || cur_vma == NULL) /* Invalid memory identify */
        return -1;

    pg_setval (caller->mm, currg->rg_start + offset, value, caller);

    return 0;
}

/**
 * @brief Paging-based memory write. First, it decodes the page number by
 * analyzing the [offset] (as virtual address). After that, it uses
 * paging-operations to get the corresponding physical address (pg_setval).
 * Finally, it issues the MEMPHY to update the corresponding frame
 * (MEMPHY_write).
 * @param proc the process requesting the write
 * @param data BYTE (char) to be written
 * @param destination region id
 * @param offset offset of Virtual Memory Area.
 * @attention the core routine is pg_getval()
 */
int
pgwrite (struct pcb_t *proc,   // Process executing the instruction
         BYTE data,            // Data to be wrttien into memory
         uint32_t destination, // Index of destination register
         uint32_t offset)
{
#ifdef IODUMP
    printf ("write val=%d ==> region=%d,offset=%d\n", data, destination,
            offset);
#ifdef PAGETBL_DUMP
    print_pgtbl (proc, 0, -1); // print max TBL
#endif
    MEMPHY_dump (proc->mram);
#endif

    return __write (proc, 0, destination, offset, data);
}

/** collect all memphy of pcb
 *@param caller: caller
 *@param vmaid: ID vm area to alloc memory region
 *@param incpgnum: number of page
 */
int
free_pcb_memph (struct pcb_t *caller)
{
    int pagenum, fpn;
    uint32_t pte;

    for (pagenum = 0; pagenum < PAGING_MAX_PGN; pagenum++)
        {
            pte = caller->mm->pgd[pagenum];

            if (!PAGING_PAGE_PRESENT (pte))
                {
                    fpn = PAGING_FPN (pte);
                    MEMPHY_put_freefp (caller->mram, fpn);
                }
            else
                {
                    fpn = PAGING_SWP (pte);
                    MEMPHY_put_freefp (caller->active_mswp, fpn);
                }
        }

    return 0;
}

/** In this context, node means region.
 * @brief Construct a new region from sbrk.
 * @param caller: process
 * @param vmaid: vm area in context
 * @param size: requested size
 * @param alignedsz: (never used)
 * @return a new region starting at sbrk and ending at sbrk + size
 */
struct vm_rg_struct *
get_vm_area_node_at_brk (struct pcb_t *caller, int vmaid, int size,
                         int alignedsz)
{
    struct vm_rg_struct *newrg;
    struct vm_area_struct *cur_vma = get_vma_by_num (caller->mm, vmaid);

    newrg = malloc (sizeof (struct vm_rg_struct));

    // new region starts at virtual memory area's sbrk
    newrg->rg_start = cur_vma->sbrk;
    newrg->rg_end = newrg->rg_start + size;

    return newrg;
}

/** validate_overlap_vm_area
 *@param: caller: caller
 *@param: vmaid: ID vm area to alloc memory region
 *@param: vmastart: vma end
 *@param: vmaend: vma end
 *
 */
/**
 * @brief Validate the the VM Area does not overlap with any other VM Area
 * region.
 * @note DEPRECATED - our project only has one VM Area. No implementation is
 * done. Always return 0.
 */
int
validate_overlap_vm_area (struct pcb_t *caller, int vmaid, int vmastart,
                          int vmaend)
{
    // struct vm_area_struct *vma = caller->mm->mmap;

    /* TODO validate the planned memory area is not overlapped */

    // NK: don't implement this.

    return 0;
}

/**
 * @brief Increase vm_end in Virtual area [vmaid] in process [caller].
 * @param caller the process requesting increment
 * @param vmaid the id of Virtual Memory Area
 * @param inc_sz size of increment
 * @return 0 if sucessful; -1 if areas overlapped or mapping to memphy failed.
 */
int
inc_vma_limit (struct pcb_t *caller, int vmaid, int inc_sz)
{
    // construct a new region
    struct vm_rg_struct *newrg = malloc (sizeof (struct vm_rg_struct));
    // let's see how much we increase
    int inc_amt = PAGING_PAGE_ALIGNSZ (inc_sz);
    // how many pages are needed
    int incnumpage = inc_amt / PAGING_PAGESZ;
    // "area" is a region starting at sbrk and ending at sbrk + size.
    // It is used to check overlapping between regions
    struct vm_rg_struct *area
        = get_vm_area_node_at_brk (caller, vmaid, inc_sz, inc_amt);
    struct vm_area_struct *cur_vma = get_vma_by_num (caller->mm, vmaid);

    int old_end = cur_vma->vm_end;

    /*Validate overlap of obtained region */
    if (validate_overlap_vm_area (caller, vmaid, area->rg_start, area->rg_end)
        < 0)
        return -1; /*Overlap and failed allocation */

    /* The obtained vm area (only)
     * now will be alloc real ram region */
    // Note: This is only done after we see no overlapping
    cur_vma->vm_end += inc_sz;
    if (vm_map_ram (caller, area->rg_start, area->rg_end, old_end, incnumpage,
                    newrg)
        < 0)
        return -1; /* Map the memory to MEMRAM */

    return 0;
}

/**
 * @brief Find a victim page (most likely to be swapped out) in page directory
 * of [mm].
 * @param mm memory mapping structure
 * @param retpgn suggested victim page
 * @return 0 if successful, -1 if page list is empty
 */
int
find_victim_page (struct mm_struct *mm, int *retpgn)
{
    struct pgn_t *pg = mm->lru_pgn;
    /* TODO: Implement the theorical mechanism to find the victim page */
    //Empty list
    if(pg == NULL)
    {
        return -1;
    }
    //list only have 1 page
    if(pg->pg_next == NULL)
    {
        retpgn = pg->pgn;
        free(pg);
        pg = NULL;
        return 0;
    }
    /**
     * find the second-last entry in the list and assign the value
     * of the last node to retpgn
    */
    while(pg->pg_next->pg_next != NULL)
    {
        pg = pg->pg_next;
    }
    retpgn = pg->pg_next->pgn;

    free (pg->pg_next);
    pg->pg_next = NULL;
    return 0;
}

/** get_free_vmrg_area - get a free vm region
 *@caller: caller
 *@vmaid: ID vm area to alloc memory region
 *@size: allocated size
 *
 */
/**
 * @brief Request a new region in VM Area using First-fit algorithm.
 * @param caller the process requesting
 * @param vmaid vm area id.
 * @param size the size of
 */
int
get_free_vmrg_area (struct pcb_t *caller, int vmaid, int size,
                    struct vm_rg_struct *newrg)
{
    // get current virtual memory area
    // we only have 1 area regarding the scope of this assignment
    struct vm_area_struct *cur_vma = get_vma_by_num (caller->mm, vmaid);

    // rgit is an iterator through the list of free regions in an area
    struct vm_rg_struct *rgit = cur_vma->vm_freerg_list;

    // if there is no free region:
    if (rgit == NULL)
        return -1;

    /* Probe uninitialized newrg */
    newrg->rg_start = newrg->rg_end = -1;

    /* Traverse on list of free vm region to find a fit space */
    while (rgit != NULL)
        {
            if (rgit->rg_start + size <= rgit->rg_end)
                { /* Current region has enough space */
                    newrg->rg_start = rgit->rg_start;
                    newrg->rg_end = rgit->rg_start + size;

                    /* Update left space in chosen region */
                    if (rgit->rg_start + size < rgit->rg_end)
                        {
                            rgit->rg_start = rgit->rg_start + size;
                        }
                    else
                        { /*Use up all space, remove current node */
                            /*Clone next rg node */
                            struct vm_rg_struct *nextrg = rgit->rg_next;

                            /*Cloning */
                            if (nextrg != NULL)
                                {
                                    rgit->rg_start = nextrg->rg_start;
                                    rgit->rg_end = nextrg->rg_end;

                                    rgit->rg_next = nextrg->rg_next;

                                    free (nextrg);
                                }
                            else
                                { /*End of free list */
                                    rgit->rg_start
                                        = rgit->rg_end; // dummy, size 0 region
                                    rgit->rg_next = NULL;
                                }
                        }
                }
            else
                {
                    rgit = rgit->rg_next; // Traverse next rg
                }
        }

    if (newrg->rg_start == -1) // new region not found
        return -1;

    return 0;
}

// #endif
