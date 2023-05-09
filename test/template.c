/**
 * @file template.c
 * @author NK
 * @brief
 *      PLEASE DO NOT CARE THIS FILE
 *
 * @note
 *      PLEASE DO NOT CARE THIS FILE
 *
 */

#include "../include/common.h"
#include "../ext/munit.h"
#include <stdio.h>
#include <stdlib.h>

/* Utilities */

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









static const MunitSuite suite = {
    "Testcases for queue: \n", /* name */
    tests,                     /* MunitTest */
    NULL,                      /* suites */
    1,                         /* iterations */
    MUNIT_SUITE_OPTION_NONE    /* options */
};









int
main (int argc, char *argv[])
{
    /**
     * @param suite pass your test suite
     * @param user_data pass into `setup func` if it's not NULL, otherwise,
     * pass into MunitResult's \p user_data_or_fixture.
     * @param other_params exactly the same as main()
     *
     * @return
     */
    return munit_suite_main (&suite, NULL, argc, argv);
}