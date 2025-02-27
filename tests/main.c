#include <munit.h>

extern MunitTest unit_ring_tests[];

static MunitSuite subsuites[] = {
    {"/ring", unit_ring_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE},
    {NULL, NULL, NULL, 1, MUNIT_SUITE_OPTION_NONE},
};

static MunitSuite suite = {
    NULL, NULL, subsuites, 1, MUNIT_SUITE_OPTION_NONE,
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

int main(int argc, char **argv)
{
    return munit_suite_main(&suite, NULL, argc, argv);
}
