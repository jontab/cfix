#include "memory.h"
#include "munit.h"
#include "tcp/client.h"
#include "tcp/server.h"
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct unit_tcp_server_context_s unit_tcp_server_context_t;

/******************************************************************************/
/* Structs                                                                   */
/******************************************************************************/

struct unit_tcp_server_context_s
{
    bool            is_done;
    pthread_mutex_t lock;
    pthread_cond_t  wake;
};

MunitResult unit_tcp_server_start(const MunitParameter params[], void *fixture);
void        unit_tcp_server_start_server_on_client(cfix_transport_t *transport, void *user_data);
void        unit_tcp_server_start_client_on_client(cfix_transport_t *transport, void *user_data);
void        unit_tcp_server_context_wait(unit_tcp_server_context_t *context);

MunitTest unit_tcp_server_tests[] = {
    {"/start", unit_tcp_server_start, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

MunitResult unit_tcp_server_start(const MunitParameter params[], void *fixture)
{
    const char *host = "0.0.0.0";
    const char *port = "8080";

    unit_tcp_server_context_t context;
    context.is_done = false;
    munit_assert_int(pthread_mutex_init(&context.lock, NULL), >=, 0);
    munit_assert_int(pthread_cond_init(&context.wake, NULL), >=, 0);

    //
    cfix_server_t *server = cfix_tcp_server_create(&(cfix_serverargs_t){
        .host = host,
        .port = port,
        .on_client = unit_tcp_server_start_server_on_client,
        .on_client_user_data = &context,
    });
    cfix_client_t *client = cfix_tcp_client_create(&(cfix_clientargs_t){
        .host = host,
        .port = port,
        .on_client = unit_tcp_server_start_client_on_client,
        .on_client_user_data = &context,
    });
    munit_assert_ptr_not_null(server);
    munit_assert_ptr_not_null(client);

    //
    munit_assert_int(cfix_server_start(server), ==, 0);
    munit_assert_int(cfix_client_start(client), ==, 0);
    sleep(1);
    unit_tcp_server_context_wait(&context);

    //
    cfix_server_stop(server);
    cfix_client_stop(client);
    DELETE(server, free);
    return MUNIT_OK;
}

void unit_tcp_server_start_server_on_client(cfix_transport_t *transport, void *user_data)
{
    unit_tcp_server_context_t *context = (unit_tcp_server_context_t *)(user_data);

    //
    char ch;
    munit_assert_int(cfix_transport_recv(transport, &ch, 1), ==, 0);
    munit_assert_char(ch, ==, 'a');

    //
    munit_assert_int(pthread_mutex_lock(&context->lock), >=, 0);
    context->is_done = true;
    munit_assert_int(pthread_cond_signal(&context->wake), >=, 0);
    munit_assert_int(pthread_mutex_unlock(&context->lock), >=, 0);

    //
    cfix_transport_free(transport);
}

void unit_tcp_server_start_client_on_client(cfix_transport_t *transport, void *user_data)
{
    munit_assert_int(cfix_transport_send(transport, "a", 1), ==, 0);
    cfix_transport_free(transport);
}

void unit_tcp_server_context_wait(unit_tcp_server_context_t *context)
{
    munit_assert_int(pthread_mutex_lock(&context->lock), >=, 0);
    while (!context->is_done)
    {
        munit_assert_int(pthread_cond_wait(&context->wake, &context->lock), >=, 0);
    }

    munit_assert_int(pthread_mutex_unlock(&context->lock), >=, 0);
}
