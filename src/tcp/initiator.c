#include "./initiator.h"
#include "./close.h"
#include "./transport.h"
#include "cfix/memory.h"
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

typedef struct cfix_tcp_initiator_s cfix_tcp_initiator_t;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct cfix_tcp_initiator_s
{
    cfix_initiator_t              base;
    char                          host[BUFSIZ];
    char                          port[BUFSIZ];
    cfix_initiator_on_transport_t on_transport;
    void                         *on_transport_user_data;
};

int  cfix_tcp_initiator_create_socket(cfix_tcp_initiator_t *self);
int  cfix_tcp_initiator_start(cfix_tcp_initiator_t *self);
void cfix_tcp_initiator_stop(cfix_tcp_initiator_t *self);

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

cfix_initiator_t *cfix_tcp_initiator_create(const cfix_initiatorargs_t *args)
{
    cfix_tcp_initiator_t *self;
    NEW(self);
    if (self)
    {
        self->base = (cfix_initiator_t){
            .start = (cfix_initiator_start_t)(cfix_tcp_initiator_start),
            .stop = (cfix_initiator_stop_t)(cfix_tcp_initiator_stop),
        };
        strncpy(self->host, args->host, sizeof(self->host));
        strncpy(self->port, args->port, sizeof(self->port));
        self->on_transport = args->on_transport;
        self->on_transport_user_data = args->on_transport_user_data;
    }

    return (cfix_initiator_t *)(self);
}

/******************************************************************************/
/* Static                                                                     */
/******************************************************************************/

int cfix_tcp_initiator_create_socket(cfix_tcp_initiator_t *self)
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

int cfix_tcp_initiator_start(cfix_tcp_initiator_t *self)
{
    int socket = cfix_tcp_initiator_create_socket(self);
    if (socket < 0)
    {
        return -1;
    }

    cfix_tcp_transport_t *transport = cfix_tcp_transport_create(socket);
    if (!transport)
    {
        return -1;
    }

    self->on_transport((cfix_transport_t *)(transport), self->on_transport_user_data);
    return 0;
}

void cfix_tcp_initiator_stop(cfix_tcp_initiator_t *self)
{
    // Nothing!
}
