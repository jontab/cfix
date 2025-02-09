#include "./acceptor.h"
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

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct cfix_tcp_acceptor_s cfix_tcp_acceptor_t;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct cfix_tcp_acceptor_s
{
    cfix_acceptor_t              base;
    char                         host[BUFSIZ];
    char                         port[BUFSIZ];
    pthread_t                    thread;
    int                          socket;
    cfix_acceptor_on_transport_t on_transpoort;
    void                        *on_transport_user_data;
};

int   cfix_tcp_acceptor_create_socket(cfix_tcp_acceptor_t *self);
int   cfix_tcp_acceptor_start(cfix_tcp_acceptor_t *self);
void  cfix_tcp_acceptor_stop(cfix_tcp_acceptor_t *self);
void *cfix_tcp_acceptor_thread_main(void *data);

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

cfix_acceptor_t *cfix_tcp_acceptor_create(const cfix_acceptorargs_t *args)
{
    cfix_tcp_acceptor_t *self;
    NEW(self);
    if (self)
    {
        self->base = (cfix_acceptor_t){
            .start = (cfix_acceptor_start_t)(cfix_tcp_acceptor_start),
            .stop = (cfix_acceptor_stop_t)(cfix_tcp_acceptor_stop),
        };
        strncpy(self->host, args->host, sizeof(self->host));
        strncpy(self->port, args->port, sizeof(self->port));
        self->socket = -1;
        self->on_transpoort = args->on_transport;
        self->on_transport_user_data = args->on_transport_user_data;
    }

    return (cfix_acceptor_t *)(self);
}

/******************************************************************************/
/* Static                                                                     */
/******************************************************************************/

int cfix_tcp_acceptor_create_socket(cfix_tcp_acceptor_t *self)
{
    struct addrinfo hints, *addrs;
    CLEAR(&hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_PASSIVE;

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

        if (setsockopt(ret, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        {
            perror("setsockopt");
            CLOSE(ret);
            continue;
        }

        if (bind(ret, p->ai_addr, p->ai_addrlen) < 0)
        {
            perror("bind");
            CLOSE(ret);
            continue;
        }

        if (listen(ret, 10) < 0)
        {
            perror("listen");
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

int cfix_tcp_acceptor_start(cfix_tcp_acceptor_t *self)
{
    self->socket = cfix_tcp_acceptor_create_socket(self);
    if (self->socket < 0)
    {
        return -1;
    }

    int ret = pthread_create(&self->thread, NULL, cfix_tcp_acceptor_thread_main, (void *)(self));
    if (ret < 0)
    {
        return -1;
    }

    return 0;
}

void cfix_tcp_acceptor_stop(cfix_tcp_acceptor_t *self)
{
    if (self->socket > 0)
    {
        CLOSE(self->socket);

        //
        if (pthread_join(self->thread, NULL) < 0)
        {
            perror("pthread_join");
        }

        CLEAR(&self->thread);
    }
}

void *cfix_tcp_acceptor_thread_main(void *data)
{
    cfix_tcp_acceptor_t *self = (cfix_tcp_acceptor_t *)(data);
    while (1)
    {
        int socket = accept(self->socket, NULL, NULL);
        if (socket < 0)
        {
            perror("accept");
            break;
        }

        cfix_tcp_transport_t *transport = cfix_tcp_transport_create(socket);
        if (!transport)
        {
            perror("cfix_tcp_transport_create");
            CLOSE(socket);
            continue;
        }

        self->on_transpoort((cfix_transport_t *)(transport), self->on_transport_user_data);
    }

    return NULL;
}
