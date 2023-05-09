/**
 * @file sample.c
 * @category Unit-test
 * @author NK
 * @brief
 *      A sample of using Munit unit-test framework
 */

#include "../ext/munit.h"
#include <stdio.h>
#include <stdlib.h>

/**
 *  Here is the structure of a test
 *  
 *      [MunitResult1, MunitResult2, MunitResult3, ...] - Many MunitResults, as test functions
 *      [MunitTest1, MunitTest2, ...] - many test funcs associated with MunitTest (to form a testcase)
 *                  |
 *                  |
 *              [MunitSuite] - a final wrapper of many testcases - as a test session
 *                  |
 *      Now, `make test-sample` to see.
 * 
 */

/**
 * @brief
 *      A specific function we wish to test.
 *
 * @return
 *      Possible return values:
 *
 *      MUNIT_OK,
 *      MUNIT_SKIP,
 *      MUNIT_FAIL,
 *      MUNIT_ERROR
 */
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

/**
 * @brief
 *      A configuration of a test (that is, the testcase).
 * 
 *      Should be declared as an array of structures (array of testcases),
 *      when there are many test funcs.
 */
MunitTest tests[] = {
    /* Testcase 1 */
    /* In documentation, you should describe exactly what the
       test does, what is its expected value, and stuf...*/
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
    /* Testcase ... */
    /*You got the idea!*/

    /*  Mark the end of the array with an entry where the test function is NULL, 
        YOU MUST ALWAYS MARK THIS "ENDING" TESTCASE TO SIGNIFY THE END OF THE
        TEST SESSION
    */
    { NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

/**
 * Ok, now let's configure the overall test session (test suite)
 * 
 * @brief
 *      A configuration of all tests.
 *      Ready to be executed via munit_suite_main.
 */
static const MunitSuite suite = {
    "Testcases for queue: \n", /* name */
    tests,                     /* MunitTest */
    NULL,                      /* suites */
    1,                         /* iterations */
    MUNIT_SUITE_OPTION_NONE    /* options */
};

/*
    Now, hit make test-sample, to see the result!
*/
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