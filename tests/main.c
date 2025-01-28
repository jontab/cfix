#include "munit.h"

extern MunitTest unit_tcp_server_tests[];

MunitSuite subsuites[] = {
    {"/tcp_server", unit_tcp_server_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE},
    {NULL, NULL, NULL, 1, MUNIT_SUITE_OPTION_NONE},
};

MunitSuite suite = {NULL, NULL, subsuites, 1, MUNIT_SUITE_OPTION_NONE};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

int main(int argc, char **argv)
{
    return munit_suite_main(&suite, NULL, argc, argv);
}
