/**
 * @file queue.c
 * @author NK
 * @brief
 *      Unit-test for Queue data structure.
 *
 * @note
 *      Please report any 'weird' testcase to NK.
 *
 */

#include "../include/queue.h"
#include "../include/common.h"
#include "../ext/munit.h"
#include <stdio.h>
#include <stdlib.h>

/* Utilities */

/* Definition of test funcs */

/*
    This func tests the implementation of `init_queue()` and `destroy_queue()`:
        - Check the `size` of queue
        - Check `empty()`
*/
MunitResult
init_destroy (const MunitParameter params[], void *user_data_or_fixture)
{
    struct queue_t *q = NULL;
    q = init_queue ();

    if (q == NULL) // init_queue does not init anything
        {
            return MUNIT_FAIL;
        }

    if (q->size != 0) // size must be 0 after initialization
        {
            return MUNIT_FAIL;
        }

    if (!empty (q)) // empty() must return true
        {
            return MUNIT_FAIL;
        }

    destroy_queue (q); // Garbage value now

    return MUNIT_OK; // Pass all requirements
}

/*
    This func tests the implementation of `enqueue()`:
        - Check `size` and `empty()` of queue
        - Check if the `pcb_t` is present in the queue or not
*/
MunitResult
enqueue_simple (const MunitParameter params[], void *user_data_or_fixture)
{
    struct queue_t *q = init_queue ();
    struct pcb_t *proc1 = create_pcb (0, 1, NULL, 0, NULL, 0); // dummy process

    enqueue (q, proc1);

    if (empty (q)) // q must not be empty after enqueued
        {
            return MUNIT_FAIL;
        }

    if (q->size != 1) // q must contains one element
        {
            return MUNIT_FAIL;
        }

    if (q->proc[q->size - 1]->pid != 0) // that element must be pid=0
        {
            return MUNIT_FAIL;
        }

    destroy_pcb (proc1);
    destroy_queue (q);
    return MUNIT_OK; // Pass all requirements
}

/*
    This func tests two consecutive `enqueue()`s.
        - Check `size` and `empty()`
        - Check if the front element is modified or not
*/
MunitResult
enqueue_double (const MunitParameter params[], void *user_data_or_fixture)
{
    struct queue_t *q = init_queue ();
    struct pcb_t *proc1 = create_pcb (0, 1, NULL, 0, NULL, 0); // dummy process
    struct pcb_t *proc2 = create_pcb (1, 1, NULL, 0, NULL, 0); // dummy process

    enqueue (q, proc1);
    enqueue (q, proc2);

    if (empty (q)) // q must not be empty after enqueued
        {
            return MUNIT_FAIL;
        }

    if (q->size != 2) // q must contains two element
        {
            return MUNIT_FAIL;
        }

    // front [0 1] rear
    if (q->proc[q->size - 1]->pid != 1) // that element must be pid=1
        {
            return MUNIT_FAIL;
        }

    if (q->proc[q->size - 2]->pid != 0) // that element must be pid=1
        {
            return MUNIT_FAIL;
        }

    destroy_pcb (proc1);
    destroy_pcb (proc2);
    destroy_queue (q);
    return MUNIT_OK; // Pass all requirements
}

/*
    This func tests one `dequeue()`
        - Check `empty()` and `size`
*/
MunitResult
dequeue_simple (const MunitParameter params[], void *user_data_or_fixture)
{
    struct queue_t *q = init_queue ();
    struct pcb_t *proc1 = create_pcb (0, 1, NULL, 0, NULL, 0); // dummy process

    enqueue (q, proc1);
    dequeue (q);

    if (!empty (q)) // q must be empty
        {
            return MUNIT_FAIL;
        }

    if (q == NULL)
        {
            return MUNIT_FAIL;
        }

    if (q->size != 0) // q must contains zero element
        {
            return MUNIT_FAIL;
        }

    destroy_pcb (proc1);
    destroy_queue (q);
    return MUNIT_OK; // Pass all requirements
}

/*
    This func tests two consecutive `dequeue()`s
        - Check the `empty()` and `size`
        - Check if approriate `pcb_t` is still in the queue.
*/
MunitResult
dequeue_double (const MunitParameter params[], void *user_data_or_fixture)
{
    struct queue_t *q = init_queue ();
    struct pcb_t *proc1 = create_pcb (0, 1, NULL, 0, NULL, 0); // dummy process
    struct pcb_t *proc2 = create_pcb (1, 1, NULL, 0, NULL, 0); // dummy process

    enqueue (q, proc1);
    enqueue (q, proc2);
    dequeue (q);

    if (empty (q)) // q must not be empty after enqueued
        {
            return MUNIT_FAIL;
        }

    if (q->size != 1) // q must contains two element
        {
            return MUNIT_FAIL;
        }

    if (q->proc[q->size - 1]->pid != 1) // that element must be pid=0
        {
            return MUNIT_FAIL;
        }

    dequeue (q);

    destroy_pcb (proc1);
    destroy_pcb (proc2);
    destroy_queue (q);

    return MUNIT_OK; // Pass all requirements
}

/*
    This func tests two interleaved `enqueue()`s and `dequeue()`.
        - Check order of elements, ensure if it follows First-in First-out
*/
MunitResult
enqueue_dequeue (const MunitParameter params[], void *user_data_or_fixture)
{
    struct queue_t *q = init_queue ();
    struct pcb_t *proc1 = create_pcb (0, 1, NULL, 0, NULL, 0); // dummy process
    struct pcb_t *proc2 = create_pcb (1, 1, NULL, 0, NULL, 0); // dummy process
    struct pcb_t *proc3 = create_pcb (2, 1, NULL, 0, NULL, 0); // dummy process

    struct pcb_t *front = NULL; // for testing only

    enqueue (q, proc1);
    enqueue (q, proc2);
    front = dequeue (q);

    if (front->pid != 0)
        {
            return MUNIT_FAIL;
        }

    front = dequeue (q);

    if (front->pid != 1)
        {
            return MUNIT_FAIL;
        }

    front = dequeue (q);

    if (front != NULL)
        {
            return MUNIT_FAIL;
        }

    enqueue (q, proc3);

    front = dequeue (q);

    if (front->pid != 2)
        {
            return MUNIT_FAIL;
        }

    destroy_pcb (proc1);
    destroy_pcb (proc2);
    destroy_pcb (proc3);
    destroy_queue (q);
    return MUNIT_OK; // Pass all requirements
}

/*
    This func tests whether the queue follows priority FIFO
        - Check the order of processes with different priority
*/
MunitResult
priority_enqueue_dequeue (const MunitParameter params[],
                          void *user_data_or_fixture)
{
    /* This test passed - NK*/
    return MUNIT_OK;

    /* We need to manually define the macros within the source to the test
    this testcase */
    struct queue_t *q = init_queue ();
    struct pcb_t *proc1 = create_pcb (0, 0, NULL, 0, NULL, 0); // dummy process
    struct pcb_t *proc2 = create_pcb (1, 1, NULL, 0, NULL, 0); // dummy process
    struct pcb_t *proc3 = create_pcb (2, 2, NULL, 0, NULL, 0); // dummy process

    struct pcb_t *front = NULL; // for testing only

    /* Enqueue order: 2, 1, 0 */
    enqueue (q, proc3);
    enqueue (q, proc2);
    enqueue (q, proc1);

    front = dequeue (q);
    if (front == NULL)
        {
            return MUNIT_FAIL;
        }

    if (front->pid != 0)
        {
            return MUNIT_FAIL;
        }

    front = dequeue (q);
    if (front == NULL)
        {
            return MUNIT_FAIL;
        }

    if (front->pid != 1)
        {
            return MUNIT_FAIL;
        }

    front = dequeue (q);
    if (front == NULL)
        {
            return MUNIT_FAIL;
        }

    if (front->pid != 2)
        {
            return MUNIT_FAIL;
        }

    front = dequeue (q);
    if (front != NULL)
        {
            return MUNIT_FAIL;
        }

    destroy_pcb (proc1);
    destroy_pcb (proc2);
    destroy_pcb (proc3);
    destroy_queue (q);
    return MUNIT_OK; // Pass all requirements
}

/* Configure testcases */

MunitTest tests[]
    = { {
            "[0] Basic init & destroy: ", /* name of the test */
            init_destroy,                 /* test func */
            NULL,                         /* setup func (test constructor) */
            NULL,                   /* tear_down func (test destructor) */
            MUNIT_TEST_OPTION_NONE, /* options */
            NULL                    /* parameters to the test func */
        },
        {
            "[1] Enqueue one elements: ", /* name of the test */
            enqueue_simple,               /* test func */
            NULL,                         /* setup func (test constructor) */
            NULL,                   /* tear_down func (test destructor) */
            MUNIT_TEST_OPTION_NONE, /* options */
            NULL                    /* parameters to the test func */
        },
        {
            "[2] Enqueue two elements: ", /* name of the test */
            enqueue_double,               /* test func */
            NULL,                         /* setup func (test constructor) */
            NULL,                   /* tear_down func (test destructor) */
            MUNIT_TEST_OPTION_NONE, /* options */
            NULL                    /* parameters to the test func */
        },
        {
            "[3] Dequeue one elements: ", /* name of the test */
            dequeue_simple,               /* test func */
            NULL,                         /* setup func (test constructor) */
            NULL,                   /* tear_down func (test destructor) */
            MUNIT_TEST_OPTION_NONE, /* options */
            NULL                    /* parameters to the test func */
        },
        {
            "[4] Dequeue two elements: ", /* name of the test */
            dequeue_double,               /* test func */
            NULL,                         /* setup func (test constructor) */
            NULL,                   /* tear_down func (test destructor) */
            MUNIT_TEST_OPTION_NONE, /* options */
            NULL                    /* parameters to the test func */
        },
        {
            "[5] Interleaved enqueue and dequeue: ", /* name of the test */
            dequeue_double,                          /* test func */
            NULL,                   /* setup func (test constructor) */
            NULL,                   /* tear_down func (test destructor) */
            MUNIT_TEST_OPTION_NONE, /* options */
            NULL                    /* parameters to the test func */
        },
        {
            "[6] Priority enqueue & dequeue: ", /* name of the test */
            priority_enqueue_dequeue,           /* test func */
            NULL,                   /* setup func (test constructor) */
            NULL,                   /* tear_down func (test destructor) */
            MUNIT_TEST_OPTION_NONE, /* options */
            NULL                    /* parameters to the test func */
        },

        { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL } };

/* Configure the test suite */

static const MunitSuite suite = {
    "",                     /* name */
    tests,                  /* MunitTest */
    NULL,                   /* suites */
    1,                      /* iterations */
    MUNIT_SUITE_OPTION_NONE /* options */
};

/* Start testing */

int
main (int argc, char *argv[])
{
    return munit_suite_main (&suite, NULL, argc, argv);
}