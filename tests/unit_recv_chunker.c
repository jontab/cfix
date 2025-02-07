#include "cfix/memory.h"
#include "cfix/message.h"
#include "context.h"
#include "munit.h"
#include "tcp/acceptor.h"
#include "tcp/initiator.h"
#include <pthread.h>
#include <stdbool.h>

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define COUNT 100000
#define MESSAGE                                                                                                        \
    "8=FIX.4.0\0019=70\00135=A\00149=BuySide\00156=SellSide\00134=1\00152=20190605-11:05:36.354\00198=0\001108="       \
    "30\00110=198\001"

MunitResult unit_recv_chunker_recv(const MunitParameter params[], void *fixture);
void        unit_recv_chunker_recv_acceptor_on_transport(cfix_transport_t *transport, void *user_data);
void        unit_recv_chunker_recv_initiator_on_transport(cfix_transport_t *transport, void *user_data);

MunitTest unit_recv_chunker_tests[] = {
    {"/recv", unit_recv_chunker_recv, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

MunitResult unit_recv_chunker_recv(const MunitParameter params[], void *fixture)
{
    const char *host = "0.0.0.0";
    const char *port = "8080";

    cfix_test_context_t ctx;
    cfix_test_context_init(&ctx);
    cfix_acceptor_t  *acceptor = cfix_tcp_acceptor_create(&(cfix_acceptorargs_t){
         .host = host,
         .port = port,
         .on_transport = unit_recv_chunker_recv_acceptor_on_transport,
         .on_transport_user_data = &ctx,
    });
    cfix_initiator_t *initiator = cfix_tcp_initiator_create(&(cfix_initiatorargs_t){
        .host = host,
        .port = port,
        .on_transport = unit_recv_chunker_recv_initiator_on_transport,
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

void unit_recv_chunker_recv_acceptor_on_transport(cfix_transport_t *transport, void *user_data)
{
    cfix_test_context_t *ctx = (cfix_test_context_t *)(user_data);

    //
    cfix_transport_recv_chunker_t chunker;
    munit_assert_int(cfix_transport_recv_chunker_init(&chunker, transport), ==, 0);
    for (int i = 0; i < COUNT; i++)
    {
        char *msg_start;
        int   msg_length;
        munit_assert_int(cfix_transport_recv_chunker_recv(&chunker, &msg_start, &msg_length), ==, 0);
        munit_assert_int(msg_length, ==, strlen(MESSAGE));
        munit_assert_int(strncmp(msg_start, MESSAGE, strlen(MESSAGE)), ==, 0);
        cfix_transport_recv_chunker_shift(&chunker, msg_length);
    }

    cfix_transport_recv_chunker_fini(&chunker);
    cfix_test_context_done(ctx);
    DELETE(transport, cfix_transport_free);
}

void unit_recv_chunker_recv_initiator_on_transport(cfix_transport_t *transport, void *user_data)
{
    for (int i = 0; i < COUNT; i++)
        munit_assert_int(cfix_transport_sendall(transport, MESSAGE, strlen(MESSAGE)), ==, 0);
    cfix_transport_free(transport);
}
