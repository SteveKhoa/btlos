/**
 * This runnable file isolates the process from the scheduler and all
 * multi-threading stuff. So that, we can focus on the core interaction between
 * a process and a memory when issuing alloc, write, read and free.
 *
 * NK sent an example on Gist a few days ago.
 *
 * It is a bit difficult to make a Munit testcase for this, so we analyze the
 * output using our bare eyes instead :).
 *
 * I preferred to write all cases in this one single file. To avoid making so
 * much makerules. However, the choice is up to you to make multiple files or just use this single file
 */
#include "common.h" // common.c
#include "cpu.h"    // cpu.c
#include "mm.h"     // mm.c mm-memphy.c mm-vm.c
#include "os-mm.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

// To run this file, create a file named _.c
// then hit: make _ deps=<dependencies>
// <dependencies> = src/common.c src/mm.c src/mm-memphy.c src/mm-vm.c src/cpu.c

/* Basic case: alloc -> write -> read -> free */
void basic ();

int
main ()
{
    basic();
    return 0;
}

void
basic ()
{
    /* == Construct a process == */
    /* Define code field */
    uint32_t code_size = 4;

    struct code_seg_t *code_ptr
        = (struct code_seg_t *)malloc (sizeof (struct code_seg_t));
    code_ptr->size = code_size;
    code_ptr->text
        = (struct inst_t *)malloc (sizeof (struct inst_t) * code_size);

    /* Code */

    code_ptr->text[0].opcode = ALLOC; // alloc
    code_ptr->text[0].arg_0 = 1023;   // size=100
    code_ptr->text[0].arg_1 = 0;      // reg=0

    code_ptr->text[1].opcode = WRITE; // write
    code_ptr->text[1].arg_0 = 123;    // data=123
    code_ptr->text[1].arg_1 = 0;      // reg=0
    code_ptr->text[1].arg_2 = 15;     // offset=15

    code_ptr->text[2].opcode = READ; // read
    code_ptr->text[2].arg_0 = 0;     // source reg=0
    code_ptr->text[2].arg_1 = 15;    // offset=15
    code_ptr->text[2].arg_2 = 1;     // dest reg=1

    code_ptr->text[3].opcode = FREE; // free
    code_ptr->text[3].arg_0 = 0;     // reg=0

    // Create a process
    struct pcb_t *proc = create_pcb (0, 0, code_ptr, 0, NULL, PAGE_SIZE);

    /* == Construct memory devices == */
    int memramsz = 1048576; // 1MB
    int memswpsz = 1048576; // 1MB

    struct memphy_struct mram;                   // RAM
    struct memphy_struct mswp[PAGING_MAX_MMSWP]; // SWAP
    init_memphy (&mram, memramsz, 1);            // Init one 1MB RAM
    for (int i = 0; i < PAGING_MAX_MMSWP; ++i)   // Init four 1MB SWAP
        init_memphy (&mswp[i], memswpsz, 0);     // rdmflg = 0 to simulate the
                                             // "slowliness" of SWAP devices

    /* == Construct Memory Management devide == */
    struct mm_struct *mm = malloc (sizeof (struct mm_struct));

    /* == Link Process with Physical devices == */
    proc->mram = &mram;
    proc->mswp = (struct memphy_struct **)&mswp;
    proc->active_mswp = &mswp[0]; // Assume that we start with the first mswp
    init_mm (mm, proc); // Init memory management devices proc->mm = mm;
    proc->mm = mm;

    run (proc); // alloc
    printf ("\tAfter alloc:\n");
    print_pgtbl (proc, 0, -1);
    MEMPHY_dump (proc->mram);
    dump_register (proc);
    printf ("\n\n\n\n");

    run (proc); // write
    printf ("\tAfter write:\n");
    print_pgtbl (proc, 0, -1);
    MEMPHY_dump (proc->mram);
    dump_register (proc);
    printf ("\n\n\n\n");

    run (proc); // read
    printf ("\tAfter read:\n");
    print_pgtbl (proc, 0, -1);
    MEMPHY_dump (proc->mram);
    dump_register (proc);
    printf ("\n\n\n\n");

    run (proc); // free
    printf ("\tAfter free:\n");
    print_pgtbl (proc, 0, -1);
    MEMPHY_dump (proc->mram);
    dump_register (proc);
    printf ("\n\n\n\n");
}
