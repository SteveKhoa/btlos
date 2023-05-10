/**
 * @file mm.c
 * @category Implementation source code
 * @brief
 *      Implementation of Page-directory operations
 */

// #ifdef MM_PAGING
/*
 * PAGING based Memory Management
 * Memory management unit mm/mm.c
 */

#include "mm.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * init_pte - Initialize PTE entry
 */
int
init_pte (uint32_t *pte,
          int pre,    // present
          int fpn,    // FPN
          int drt,    // dirty
          int swp,    // swap
          int swptyp, // swap type
          int swpoff) // swap offset
{
    if (pre != 0)
        {
            if (swp == 0)
                { // Non swap ~ page online
                    if (fpn == 0)
                        return -1; // Invalid setting

                    /* Valid setting with FPN */
                    SETBIT (*pte, PAGING_PTE_PRESENT_MASK);
                    CLRBIT (*pte, PAGING_PTE_SWAPPED_MASK);
                    CLRBIT (*pte, PAGING_PTE_DIRTY_MASK);

                    SETVAL (*pte, fpn, PAGING_PTE_FPN_MASK,
                            PAGING_PTE_FPN_LOBIT);
                }
            else
                { // page swapped
                    SETBIT (*pte, PAGING_PTE_PRESENT_MASK);
                    SETBIT (*pte, PAGING_PTE_SWAPPED_MASK);
                    CLRBIT (*pte, PAGING_PTE_DIRTY_MASK);

                    SETVAL (*pte, swptyp, PAGING_PTE_SWPTYP_MASK,
                            PAGING_PTE_SWPTYP_LOBIT);
                    SETVAL (*pte, swpoff, PAGING_PTE_SWPOFF_MASK,
                            PAGING_PTE_SWPOFF_LOBIT);
                }
        }

    return 0;
}

/*
 * pte_set_swap - Set PTE entry for swapped page
 * @pte    : target page table entry (PTE)
 * @swptyp : swap type
 * @swpoff : swap offset
 */
int
pte_set_swap (uint32_t *pte, int swptyp, int swpoff)
{
    SETBIT (*pte, PAGING_PTE_PRESENT_MASK);
    SETBIT (*pte, PAGING_PTE_SWAPPED_MASK);

    SETVAL (*pte, swptyp, PAGING_PTE_SWPTYP_MASK, PAGING_PTE_SWPTYP_LOBIT);
    SETVAL (*pte, swpoff, PAGING_PTE_SWPOFF_MASK, PAGING_PTE_SWPOFF_LOBIT);

    return 0;
}

/*
 * pte_set_swap - Set PTE entry for on-line page
 * @pte   : target page table entry (PTE)
 * @fpn   : frame page number (FPN)
 */
int
pte_set_fpn (uint32_t *pte, int fpn)
{
    SETBIT (*pte, PAGING_PTE_PRESENT_MASK);
    CLRBIT (*pte, PAGING_PTE_SWAPPED_MASK);

    SETVAL (*pte, fpn, PAGING_PTE_FPN_MASK, PAGING_PTE_FPN_LOBIT);

    return 0;
}

/** vmap_page_range - map a range of page at aligned address
 * @param caller: process
 * @param addr: start address which is aligned to pagesz
 * @param pgnum: number of pages need mapping
 * @param frames: list of mapped frames
 * @param ret_rg: return mapped region, the mapped fp (no guarantee all pages
 * mapped)
 */
int
vmap_page_range (struct pcb_t *caller, int addr, int pgnum,
                 struct framephy_struct *frames, struct vm_rg_struct *ret_rg)
{
    // @note NK : now we don't need [frames] and [ret_rg]
    // @note NK : what we really need in this function is to make all the pages
    // belonging to this func available (or "present")

    // uint32_t * pte = malloc(sizeof(uint32_t));
    // struct framephy_struct *fpit = malloc (sizeof (struct framephy_struct));
    // int  fpn;
    // int pgit = 0;

    //    ret_rg->rg_end = ret_rg->rg_start
    //        = addr; // at least the very first space is usable

    /* TODO map range of frame to address space
     *      [addr to addr + pgnum * PAGING_PAGESZ
     *      in page table caller->mm->pgd[]
     */

    // @note NK : if [pgnum] is zero, the number of frames needed for this func
    // is not sufficient. Therefore, we must perform a pg_getpage() (which uses
    // swapping internally) to get the fpn (associated with pgn) we need.

    // @note NK : deprecate this part, i rewrite a new one.
    // int pgn;
    // fpit = frames;
    // while (fpit)
    //     {
    //         // get page number from addr
    //         pgn = PAGING_PGN (addr);
    //         pgit = pgn;
    //         PAGING_PTE_SET_PRESENT (caller->mm->pgd[pgn]);
    //         pte_set_fpn (&caller->mm->pgd[pgn], fpit->fpn);
    //         fpit = fpit->fp_next;
    //         addr += PAGING_PAGESZ;
    //         /* Tracking for later page replacement activities (if needed)
    //          * Enqueue new usage page */
    //         enlist_pgn_node (&caller->mm->lru_pgn, pgn + pgit);
    //     }

    // @note NK : this is my new implementation:
    for (int i = 0; i < pgnum; ++i) // Try to get [pgnum] frames for all pgn
        {
            int pgn;
            int fpn;

            pgn = PAGING_PGN (addr); // get the page

            if (pg_getpage (caller->mm, pgn, &fpn, caller) != 0)
                {
                    printf ("Error: in mm.c / vmap_page_range() :\n");
                    printf ("pg_getpage() is not sucessful.\n");
                    return -1;
                } // Try to get a frame for [pgn]

            addr += PAGING_PAGESZ; // go to the beginning of nextpage

            /* Tracking for later page replacement activities */
            // enlist_pgn_node (&caller->mm->lru_pgn, pgn);
            // @note NK : no need to enlist this pgn, because pg_getpage()
            // already did that for us
        }

    return 0;
}

/**
 * @brief Enlist [fpn] to [frm_lst] linked list.
 * @author NK
 */
int
enlist_framephy (struct mm_struct *mm, struct framephy_struct **frm_lst,
                 int fpn)
{
    if (frm_lst == NULL)
        return -1;

    struct framephy_struct *p = *frm_lst;

    if (p == NULL) // if Frame list currently empty
        {
            *frm_lst = malloc (sizeof (struct framephy_struct));
            (*frm_lst)->fpn = fpn;
            (*frm_lst)->fp_next = NULL;
            (*frm_lst)->owner = mm;
        }
    else // if Frame is NOT empty
        {

            while (p->fp_next != NULL)
                {
                    p = p->fp_next;
                }

            p->fp_next = malloc (sizeof (struct framephy_struct));
            p->fp_next->fpn = fpn;
            p->fp_next->fp_next = NULL;
            p->fp_next->owner = mm;
        }

    return 0;
}

/**
 * @brief Try to get [req_pgnum] free frames.
 * @param caller the process requesting
 * @param req_pgnum requested number of pages (also number of frames requested)
 * @param frm_lst store the new free frames list. (frm_lst should be empty
 * before passed into)
 *
 * @note If not enough frames, [req_pgnum] will be the number of remaining
 * pages.
 * @attention DEPRECATED
 */
int
alloc_pages_range (struct pcb_t *caller, int *req_pgnum,
                   struct framephy_struct **frm_lst)
{
    int pgit, fpn;
    // struct framephy_struct *newfp_str;

    for (pgit = 0; pgit < *req_pgnum; pgit++)
        {
            if (MEMPHY_get_freefp (caller->mram, &fpn) == 0)
                {
                    // Add fpn to frm_lst
                    enlist_framephy (caller->mm, frm_lst, fpn);
                }
            else
                { // ERROR CODE of obtaining somes but not enough frames.

                    /* DEPRECATE THIS METHOD: */
                    // If not enough frames, we give back all frames to MEMPHY
                    // and issue error code (-1)

                    // for (struct framephy_struct *p = *frm_lst; p != NULL;
                    //      p = p->fp_next)
                    //     {
                    //         int id = p->fpn;
                    //         MEMPHY_put_freefp (caller->mram, id);
                    //     }

                    // // Dealloc resources of frm_lst
                    // struct framephy_struct *p = *frm_lst;
                    // while (p != NULL)
                    //     {
                    //         struct framephy_struct *pnext = p->fp_next;
                    //         free (p);
                    //         p = pnext;
                    //     }

                    // *frm_lst = NULL;
                    // return -1;

                    /* NEW METHOD: Try to utilize swap space to get new free
                     * frames */
                    // However, this is not the job of this func
                    // so, req_pgnum will be the number of remaining pages.
                    // This job will be done in vmap_page_range()

                    *req_pgnum = *req_pgnum - pgit; // return req_pgnum
                    return 0;
                }
        }

    return 0;
}

/**
 * @brief do the mapping all vm are to ram storage device
 * @param caller caller
 * @param astart vm area start
 * @param aend vm area end
 * @param mapstart start mapping point
 * @param incpgnum number of mapped page
 * @param ret_rg returned region
 */
int
vm_map_ram (struct pcb_t *caller, int astart, int aend, int mapstart,
            int incpgnum, struct vm_rg_struct *ret_rg)
{
    struct framephy_struct *frm_lst = NULL;
    int ret_alloc;

    /*@bksysnet: author provides a feasible solution of getting frames
     *FATAL logic in here, wrong behaviour if we have not enough page
     *i.e. we request 1000 frames meanwhile our RAM has size of 3 frames
     *Don't try to perform that case in this simple work, it will result
     *in endless procedure of swap-off to get frame and we have not provide
     *duplicate control mechanism, keep it simple
     */

    // ret_alloc = alloc_pages_range (caller, incpgnum, &frm_lst);
    // @note NK : we don't need this because we have pg_getpg().
    ret_alloc = 0;

    if (ret_alloc < 0 && ret_alloc != -3000)
        return -1;

    /* Out of memory */
    if (ret_alloc == -3000)
        {
#ifdef MMDBG
            printf ("OOM: vm_map_ram out of memory \n");
#endif
            return -1;
        }

    /* If there is enough memory available but it is split between RAM and
     * swap, the code performs swapping operations to bring all the required
     * data into RAM. This ensures that the entire data set is resident in RAM
     * rather than being partially stored in swap.
     * */
    return vmap_page_range (caller, mapstart, incpgnum, frm_lst, ret_rg);
    // @note NK: vmap_page_range() uses pg_getpage().

    // return 0;
}

/**
 * @brief Read from [srcfpn] frame and write to [dstfpn] frame.
 * @param mpsrc source memphy
 * @param mpdst destination memphy
 * @param srcfpn source frame id
 * @param dstfon destination frame id.
 * @return 0 always sucessful.
 */
int
__swap_cp_page (struct memphy_struct *mpsrc, int srcfpn,
                struct memphy_struct *mpdst, int dstfpn)
{
    int cellidx;
    int addrsrc, addrdst;
    for (cellidx = 0; cellidx < PAGING_PAGESZ; cellidx++)
        {
            addrsrc = srcfpn * PAGING_PAGESZ + cellidx;
            addrdst = dstfpn * PAGING_PAGESZ + cellidx;

            BYTE data;
            MEMPHY_read (mpsrc, addrsrc, &data);
            MEMPHY_write (mpdst, addrdst, data);
        }

    return 0;
}

/*
 *Initialize a empty Memory Management instance
 * @mm:     self mm
 * @caller: mm owner
 */
int
init_mm (struct mm_struct *mm, struct pcb_t *caller)
{
    struct vm_area_struct *vma = malloc (sizeof (struct vm_area_struct));

    mm->pgd = malloc (PAGING_MAX_PGN * sizeof (uint32_t));

    /* By default the owner comes with at least one vma */
    vma->vm_id = 1;
    vma->vm_start = 0;
    vma->vm_end = vma->vm_start;
    vma->sbrk = vma->vm_start;
    struct vm_rg_struct *first_rg = init_vm_rg (vma->vm_start, vma->vm_end);
    enlist_vm_rg_node (&vma->vm_freerg_list, first_rg);

    vma->vm_next = NULL;
    vma->vm_mm = mm; /*point back to vma owner */

    mm->mmap = vma;

    // Initialize the symbol region tables
    // Avoid garbage values
    for (int i = 0; i < PAGING_MAX_SYMTBL_SZ; ++i)
        {
            mm->symrgtbl[i].rg_start = -1;
            mm->symrgtbl[i].rg_end = -1;
            mm->symrgtbl[i].rg_next = NULL;
        }
    for (int i = 0; i < PAGING_MAX_PGN; ++i)
        {
            mm->pgd[i] = 0;
        }

    return 0;
}

/**
 * @brief init a vm_rg_struct
 */
struct vm_rg_struct *
init_vm_rg (int rg_start, int rg_end)
{
    struct vm_rg_struct *rgnode = malloc (sizeof (struct vm_rg_struct));

    rgnode->rg_start = rg_start;
    rgnode->rg_end = rg_end;
    rgnode->rg_next = NULL;

    return rgnode;
}

/**
 * @brief add a [rgnode] to [rglist].
 */
int
enlist_vm_rg_node (struct vm_rg_struct **rglist, struct vm_rg_struct *rgnode)
{
    rgnode->rg_next = *rglist;
    *rglist = rgnode;

    return 0;
}

/**
 * @brief: pgn to the list but if pgn value is already in the list
 * then move the node to the top instead
 */

int
enlist_pgn_node (struct pgn_t **plist, int pgn)
{
    struct pgn_t *pnode = *plist;
    struct pgn_t *prev = NULL;
    // Find pgn in the list
    while (pnode != NULL)
        {
            if (pnode->pgn == pgn)
                {
                    if (prev != NULL)
                        {
                            // move the node to the front
                            prev->pg_next = pnode->pg_next;
                            pnode->pg_next = *plist;
                            *plist = pnode;
                        }
                    return 0;
                }
            prev = pnode;
            pnode = pnode->pg_next;
        }
    // pgn not found, create and add the new node to the front of the list
    pnode = malloc (sizeof (struct pgn_t));
    pnode->pgn = pgn;
    pnode->pg_next = *plist;
    *plist = pnode;

    return 0;
}

int
print_list_fp (struct framephy_struct *ifp)
{
    struct framephy_struct *fp = ifp;

    printf ("print_list_fp: ");
    if (fp == NULL)
        {
            printf ("NULL list\n");
            return -1;
        }
    printf ("\n");
    while (fp != NULL)
        {
            printf ("fp[%d]\n", fp->fpn);
            fp = fp->fp_next;
        }
    printf ("\n");
    return 0;
}

int
print_list_rg (struct vm_rg_struct *irg)
{
    struct vm_rg_struct *rg = irg;

    printf ("print_list_rg: ");
    if (rg == NULL)
        {
            printf ("NULL list\n");
            return -1;
        }
    printf ("\n");
    while (rg != NULL)
        {
            printf ("rg[%ld->%ld]\n", rg->rg_start, rg->rg_end);
            rg = rg->rg_next;
        }
    printf ("\n");
    return 0;
}

int
print_list_vma (struct vm_area_struct *ivma)
{
    struct vm_area_struct *vma = ivma;

    printf ("print_list_vma: ");
    if (vma == NULL)
        {
            printf ("NULL list\n");
            return -1;
        }
    printf ("\n");
    while (vma != NULL)
        {
            printf ("va[%ld->%ld]\n", vma->vm_start, vma->vm_end);
            vma = vma->vm_next;
        }
    printf ("\n");
    return 0;
}

int
print_list_pgn (struct pgn_t *ip)
{
    printf ("print_list_pgn: ");
    if (ip == NULL)
        {
            printf ("NULL list\n");
            return -1;
        }
    printf ("\n");
    while (ip != NULL)
        {
            printf ("va[%d]-\n", ip->pgn);
            ip = ip->pg_next;
        }
    printf ("n");
    return 0;
}

int
print_pgtbl (struct pcb_t *caller, uint32_t start, uint32_t end)
{
    int pgn_start, pgn_end;
    int pgit;

    if (end == -1)
        {
            pgn_start = 0;
            struct vm_area_struct *cur_vma = get_vma_by_num (caller->mm, 0);
            end = cur_vma->vm_end;
        }
    pgn_start = PAGING_PGN (start);
    pgn_end = PAGING_PGN (end);
    flockfile (stdout); // To avoid the dump messages
                        // interleaved by external messages
    printf ("print_pgtbl: %d - %d", start, end);
    if (caller == NULL)
        {
            printf ("NULL caller\n");
            return -1;
        }
    printf ("\n");

    for (pgit = pgn_start; pgit < pgn_end; pgit++)
        {
            printf ("%08ld: %08x\n", pgit * sizeof (uint32_t),
                    caller->mm->pgd[pgit]);
        }
    funlockfile (stdout); // Follows the above flockfile()
    return 0;
}

// #endif
