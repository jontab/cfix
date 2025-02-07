#include "cfix/memory.h"
#include "context.h"
#include "munit.h"
#include "tcp/acceptor.h"
#include "tcp/initiator.h"
#include <pthread.h>
#include <stdbool.h>

MunitResult unit_tcp_acceptor_start(const MunitParameter params[], void *fixture);
void        unit_tcp_acceptor_start_acceptor_on_transport(cfix_transport_t *transport, void *user_data);
void        unit_tcp_acceptor_start_initiator_on_transport(cfix_transport_t *transport, void *user_data);

MunitTest unit_tcp_acceptor_tests[] = {
    {"/start", unit_tcp_acceptor_start, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

MunitResult unit_tcp_acceptor_start(const MunitParameter params[], void *fixture)
{
    const char *host = "0.0.0.0";
    const char *port = "8080";

    cfix_test_context_t ctx;
    cfix_test_context_init(&ctx);
    cfix_acceptor_t  *acceptor = cfix_tcp_acceptor_create(&(cfix_acceptorargs_t){
         .host = host,
         .port = port,
         .on_transport = unit_tcp_acceptor_start_acceptor_on_transport,
         .on_transport_user_data = &ctx,
    });
    cfix_initiator_t *initiator = cfix_tcp_initiator_create(&(cfix_initiatorargs_t){
        .host = host,
        .port = port,
        .on_transport = unit_tcp_acceptor_start_initiator_on_transport,
        .on_transport_user_data = &ctx,
    });
    munit_assert_ptr_not_null(acceptor);
    munit_assert_ptr_not_null(initiator);

    //
    munit_assert_int(cfix_acceptor_start(acceptor), ==, 0);
    munit_assert_int(cfix_initiator_start(initiator), ==, 0);
    cfix_test_context_wait(&ctx);

    //
    cfix_acceptor_stop(acceptor);
    cfix_initiator_stop(initiator);
    DELETE(acceptor, free);
    return MUNIT_OK;
}

void unit_tcp_acceptor_start_acceptor_on_transport(cfix_transport_t *transport, void *user_data)
{
    cfix_test_context_t *ctx = (cfix_test_context_t *)(user_data);

    //
    char ch;
    munit_assert_int(cfix_transport_recvall(transport, &ch, 1), ==, 0);
    munit_assert_char(ch, ==, 'a');

    //
    cfix_test_context_done(ctx);
    cfix_transport_free(transport);
}

void unit_tcp_acceptor_start_initiator_on_transport(cfix_transport_t *transport, void *user_data)
{
    munit_assert_int(cfix_transport_sendall(transport, "a", 1), ==, 0);
    DELETE(transport, cfix_transport_free);
}
