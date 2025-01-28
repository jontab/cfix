#include "./client.h"
#include "../memory.h"
#include "./close.h"
#include "./transport.h"
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#define CLOSE(X)                                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        close(X);                                                                                                      \
        X = -1;                                                                                                        \
    } while (0)

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct cfix_tcp_client_s cfix_tcp_client_t;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct cfix_tcp_client_s
{
    cfix_client_t           base;
    char                    host[BUFSIZ];
    char                    port[BUFSIZ];
    cfix_client_on_client_t on_client;
    void                   *on_client_user_data;
};

int  cfix_tcp_client_create_socket(cfix_tcp_client_t *self);
int  cfix_tcp_client_start(cfix_tcp_client_t *self);
void cfix_tcp_client_stop(cfix_tcp_client_t *self);

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

cfix_client_t *cfix_tcp_client_create(const cfix_clientargs_t *args)
{
    cfix_tcp_client_t *self;
    NEW(self);
    if (self)
    {
        self->base = (cfix_client_t){
            .start = (cfix_client_start_t)(cfix_tcp_client_start),
            .stop = (cfix_client_stop_t)(cfix_tcp_client_stop),
        };
        strncpy(self->host, args->host, sizeof(self->host));
        strncpy(self->port, args->port, sizeof(self->port));
        self->on_client = args->on_client;
        self->on_client_user_data = args->on_client_user_data;
    }

    return (cfix_client_t *)(self);
}

/******************************************************************************/
/* Static                                                                     */
/******************************************************************************/

int cfix_tcp_client_create_socket(cfix_tcp_client_t *self)
{
    struct addrinfo hints, *addrs;
    CLEAR(&hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;

    //
    int ret = getaddrinfo(self->host, self->port, &hints, &addrs);
    if (ret)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        return -1;
    }

    for (struct addrinfo *p = addrs; p; p = p->ai_next)
    {
        ret = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (ret < 0)
        {
            perror("socket");
            continue;
        }

        if (connect(ret, p->ai_addr, p->ai_addrlen) < 0)
        {
            perror("connect");
            CLOSE(ret);
            continue;
        }

        break;
    }

    DELETE(addrs, freeaddrinfo);

    //
    if (ret < 0)
    {
        return -1;
    }

    return ret;
}

int cfix_tcp_client_start(cfix_tcp_client_t *self)
{
    int socket = cfix_tcp_client_create_socket(self);
    if (socket < 0)
    {
        return -1;
    }

    cfix_tcp_transport_t *transport = cfix_tcp_transport_create(socket);
    if (!transport)
    {
        return -1;
    }

    self->on_client((cfix_transport_t *)(transport), self->on_client_user_data);
    return 0;
}

void cfix_tcp_client_stop(cfix_tcp_client_t *self)
{
    // Nothing!
}
