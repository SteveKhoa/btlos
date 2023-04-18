/**
 * @file queue.c
 * @category Executable file
 * @author NK
 * @brief
 *      Unit-test for Queue data-structure. Using Munit.h
 */

#include "munit.h"
#include <stdio.h>
#include <stdlib.h>

MunitResult
sample (const MunitParameter params[], void *user_data_or_fixture)
{
    munit_assert_double(12.2, >, 5);
}

int
main ()
{
    return 0;
}