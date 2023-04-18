/**
 * @file queue.c
 * @author NK
 * @brief 
 *      Unit-test for Queue data structure.
 * 
 */

#include "munit.h"
#include <stdio.h>
#include <stdlib.h>

/* Definition of test funcs */

MunitResult
test1 (const MunitParameter params[], void *user_data_or_fixture)
{
    /* Do something, such as comparing 1 == 2 ? */
    return MUNIT_OK;
}

MunitResult
test2 (const MunitParameter params[], void *user_data_or_fixture)
{
    /* Let this test failed purposefully */
    return MUNIT_FAIL;
}

MunitResult
test3 (const MunitParameter params[], void *user_data_or_fixture)
{
    /* Skip this test if we are not interested. */
    return MUNIT_SKIP;
}

/* Configure testcases */

MunitTest tests[] = {
    {
        "Testcase 1: ",         /* name of the test */
        test1,                  /* test func */
        NULL,                   /* setup func (test constructor) */
        NULL,                   /* tear_down func (test destructor) */
        MUNIT_TEST_OPTION_NONE, /* options */
        NULL                    /* parameters to the test func */
    },
    /* Testcase 2 */
    {
        "Testcase 2: ",         /* name of the test */
        test2,                  /* test func */
        NULL,                   /* setup func (test constructor) */
        NULL,                   /* tear_down func (test destructor) */
        MUNIT_TEST_OPTION_NONE, /* options */
        NULL                    /* parameters to the test func */
    },
    /* Testcase 3 */
    {
        "Testcase 3: ",         /* name of the test */
        test3,                  /* test func */
        NULL,                   /* setup func (test constructor) */
        NULL,                   /* tear_down func (test destructor) */
        MUNIT_TEST_OPTION_NONE, /* options */
        NULL                    /* parameters to the test func */
    },

    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

/* Configure the test suite */

static const MunitSuite suite = {
    "Testcases for queue: \n", /* name */
    tests,                     /* MunitTest */
    NULL,                      /* suites */
    1,                         /* iterations */
    MUNIT_SUITE_OPTION_NONE    /* options */
};

/* Start testing */

int
main (int argc, char *argv[])
{
    // return munit_suite_main (&suite, NULL, argc, argv);
    
    return 0;
}