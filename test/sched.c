/**
 * @file sched.c
 * @author NK
 * @brief
 *      Unit-test for MLQ scheduler
 *      (implemented in sched.c and interface in sched.h)
 *
 */

#include "../include/sched.h"
#include "../src/sched.c"
#include "../ext/munit.h"
#include <stdio.h>
#include <stdlib.h>

/* No documentation about this test at the moment
    Please message NK if you found some tests are wrong or don't understand
    the tests themselves.
*/

MunitResult
init_finish (const MunitParameter params[], void *user_data_or_fixture)
{
    if (queue_empty () != 1)
        {
            return MUNIT_FAIL;
        }

    init_scheduler ();

    if (queue_empty () != 1)
        {
            return MUNIT_FAIL;
        }

    finish_scheduler ();

    if (queue_empty () != 1)
        {
            return MUNIT_FAIL;
        }

    return MUNIT_OK; // Pass all requirements
}

MunitResult
putproc_simple (const MunitParameter params[], void *user_data_or_fixture)
{
    init_scheduler ();
    struct pcb_t *proc1 = create_pcb (0, 0, NULL, 0, NULL, 0); // prio = 0
    struct pcb_t *proc2 = create_pcb (1, 1, NULL, 0, NULL, 0); // prio = 1
    struct pcb_t *proc3 = create_pcb (2, 9, NULL, 0, NULL, 0); // prio = 9

    put_proc (proc1);
    put_proc (proc2);
    put_proc (proc3);

    printf("fd");

    if (queue_empty () != -1)
        {
            return MUNIT_FAIL;
        }

    destroy_pcb (proc1);
    destroy_pcb (proc2);
    destroy_pcb (proc3);
    finish_scheduler ();
    return MUNIT_OK; // Pass all requirements
}

MunitResult
putproc_trick_1 (const MunitParameter params[], void *user_data_or_fixture)
{
    init_scheduler ();
    struct pcb_t *proc1 = create_pcb (0, -1, NULL, 0, NULL, 0); // prio = -1

    put_proc (proc1);

    if (queue_empty () != 1)
        {
            return MUNIT_FAIL;
        }

    destroy_pcb (proc1);
    finish_scheduler ();
    return MUNIT_OK; // Pass all requirements
}

MunitResult
putproc_trick_2 (const MunitParameter params[], void *user_data_or_fixture)
{
    init_scheduler ();
    struct pcb_t *proc1
        = create_pcb (0, MAX_PRIO, NULL, 0, NULL, 0); // prio = MAX_PRIO

    put_proc (proc1);

    if (queue_empty () != 1)
        {
            return MUNIT_FAIL;
        }

    destroy_pcb (proc1);
    finish_scheduler ();
    return MUNIT_OK; // Pass all requirements
}

MunitResult
putproc_same_priority (const MunitParameter params[],
                       void *user_data_or_fixture)
{
    init_scheduler ();
    struct pcb_t *proc1 = create_pcb (0, 0, NULL, 0, NULL, 0); // prio = 0
    struct pcb_t *proc2 = create_pcb (1, 0, NULL, 0, NULL, 0); // prio = 0
    struct pcb_t *proc3 = create_pcb (2, 0, NULL, 0, NULL, 0); // prio = 0

    put_proc (proc1);
    put_proc (proc2);
    put_proc (proc3);

    if (queue_empty () != -1)
        {
            return MUNIT_FAIL;
        }

    destroy_pcb (proc1);
    destroy_pcb (proc2);
    destroy_pcb (proc3);
    finish_scheduler ();
    return MUNIT_OK; // Pass all requirements
}

MunitResult
getproc_simple (const MunitParameter params[], void *user_data_or_fixture)
{
    init_scheduler ();
    struct pcb_t *proc1 = create_pcb (0, 0, NULL, 0, NULL, 0); // prio = 0
    struct pcb_t *proc2 = create_pcb (1, 1, NULL, 0, NULL, 0); // prio = 1
    struct pcb_t *proc3 = create_pcb (2, 9, NULL, 0, NULL, 0); // prio = 9

    put_proc (proc3);
    put_proc (proc2);
    put_proc (proc1);

    struct pcb_t *front = NULL;
    front = get_proc ();

    if (front == NULL)
        {
            return MUNIT_FAIL;
        }

    if (front->pid != 0)
        {
            return MUNIT_FAIL;
        }

    front = get_proc ();

    if (front == NULL)
        {
            return MUNIT_FAIL;
        }

    if (front->pid != 1)
        {
            return MUNIT_FAIL;
        }

    front = get_proc ();

    if (front == NULL)
        {
            return MUNIT_FAIL;
        }

    if (front->pid != 2)
        {
            return MUNIT_FAIL;
        }

    destroy_pcb (proc1);
    destroy_pcb (proc2);
    destroy_pcb (proc3);
    finish_scheduler ();
    return MUNIT_OK; // Pass all requirements
}

MunitResult
getproc_complex (const MunitParameter params[], void *user_data_or_fixture)
{
    init_scheduler ();
    struct pcb_t *proc1 = create_pcb (0, 0, NULL, 0, NULL, 0);
    struct pcb_t *proc2 = create_pcb (1, 0, NULL, 0, NULL, 0);
    struct pcb_t *proc3 = create_pcb (2, 0, NULL, 0, NULL, 0);
    struct pcb_t *proc4 = create_pcb (3, 0, NULL, 0, NULL, 0);
    struct pcb_t *proc5 = create_pcb (4, 0, NULL, 0, NULL, 0);
    struct pcb_t *proc6 = create_pcb (5, 0, NULL, 0, NULL, 0);
    struct pcb_t *proc7 = create_pcb (6, 0, NULL, 0, NULL, 0);

    put_proc (proc1);
    put_proc (proc2);
    put_proc (proc3);
    put_proc (proc4);
    put_proc (proc5);
    put_proc (proc6);
    put_proc (proc7);

    /* Order of MLQ after pushed */
    /*
        prio=1  rear-> [4,3,0   ] ->front
        prio=2  rear-> [1       ] ->front
        prio=3  rear-> [5,2     ] ->front
        prio=4  rear-> [6       ] ->front
        prio=...
    */
    /*
         Because of the above order, we expect the output will be:
         0, 3, 4, 1, 5, 2, 6
    */

    int order[] = { 0, 3, 4, 1, 5, 2, 6 };

    for (int i = 0; i < 7; ++i)
        {
            struct pcb_t *get = get_proc ();

            if (get == NULL)
                {
                    return MUNIT_FAIL;
                }

            if (get->pid != order[i])
                {
                    return MUNIT_FAIL;
                }
        }

    if (get_proc () != NULL)
        {
            return MUNIT_FAIL;
        }

    destroy_pcb (proc1);
    destroy_pcb (proc2);
    destroy_pcb (proc3);
    destroy_pcb (proc4);
    destroy_pcb (proc5);
    destroy_pcb (proc6);
    destroy_pcb (proc7);
    finish_scheduler ();
    return MUNIT_OK; // Pass all requirements
}

MunitTest tests[]
    = { {
            "Testcase 1: ",         /* name of the test */
            init_finish,            /* test func */
            NULL,                   /* setup func (test constructor) */
            NULL,                   /* tear_down func (test destructor) */
            MUNIT_TEST_OPTION_NONE, /* options */
            NULL                    /* parameters to the test func */
        },
        {
            "Testcase 2: ",         /* name of the test */
            putproc_simple,         /* test func */
            NULL,                   /* setup func (test constructor) */
            NULL,                   /* tear_down func (test destructor) */
            MUNIT_TEST_OPTION_NONE, /* options */
            NULL                    /* parameters to the test func */
        },
        {
            "Testcase 3: ",         /* name of the test */
            putproc_trick_1,        /* test func */
            NULL,                   /* setup func (test constructor) */
            NULL,                   /* tear_down func (test destructor) */
            MUNIT_TEST_OPTION_NONE, /* options */
            NULL                    /* parameters to the test func */
        },
        {
            "Testcase 4: ",         /* name of the test */
            putproc_trick_2,        /* test func */
            NULL,                   /* setup func (test constructor) */
            NULL,                   /* tear_down func (test destructor) */
            MUNIT_TEST_OPTION_NONE, /* options */
            NULL                    /* parameters to the test func */
        },
        {
            "Testcase 5: ",         /* name of the test */
            putproc_same_priority,        /* test func */
            NULL,                   /* setup func (test constructor) */
            NULL,                   /* tear_down func (test destructor) */
            MUNIT_TEST_OPTION_NONE, /* options */
            NULL                    /* parameters to the test func */
        },
        {
            "Testcase 6: ",         /* name of the test */
            getproc_simple,        /* test func */
            NULL,                   /* setup func (test constructor) */
            NULL,                   /* tear_down func (test destructor) */
            MUNIT_TEST_OPTION_NONE, /* options */
            NULL                    /* parameters to the test func */
        },
        {
            "Testcase 7: ",         /* name of the test */
            getproc_complex,        /* test func */
            NULL,                   /* setup func (test constructor) */
            NULL,                   /* tear_down func (test destructor) */
            MUNIT_TEST_OPTION_NONE, /* options */
            NULL                    /* parameters to the test func */
        },
        { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } };

static const MunitSuite suite = {
    "",                     /* name */
    tests,                  /* MunitTest */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

int
main (int argc, char *argv[])
{
    return munit_suite_main (&suite, NULL, argc, argv);
}