#include "munit.h"

extern MunitTest unit_recv_chunker_tests[];
extern MunitTest unit_send_chunker_tests[];
extern MunitTest unit_tcp_acceptor_tests[];

MunitSuite subsuites[] = {
    {"/recv_chunker", unit_recv_chunker_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE},
    {"/send_chunker", unit_send_chunker_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE},
    {"/tcp_acceptor", unit_tcp_acceptor_tests, NULL, 1, MUNIT_SUITE_OPTION_NONE},
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
