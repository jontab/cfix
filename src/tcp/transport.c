#include "./transport.h"
#include "cfix/memory.h"
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#define CLOSE(X)                                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        close(X);                                                                                                      \
        X = -1;                                                                                                        \
    } while (0)

int  cfix_tcp_transport_send(cfix_tcp_transport_t *self, void *data, size_t length);
int  cfix_tcp_transport_recv(cfix_tcp_transport_t *self, void *data, size_t length);
void cfix_tcp_transport_free(cfix_tcp_transport_t *self);

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

cfix_tcp_transport_t *cfix_tcp_transport_create(int socket)
{
    cfix_tcp_transport_t *self;
    NEW(self);
    if (self)
    {
        self->base = (cfix_transport_t){
            .send = (cfix_transport_send_t)(cfix_tcp_transport_send),
            .recv = (cfix_transport_recv_t)(cfix_tcp_transport_recv),
            .free = (cfix_transport_free_t)(cfix_tcp_transport_free),
        };
        self->socket = socket;
    }

    return self;
}

/******************************************************************************/
/* Static                                                                     */
/******************************************************************************/

int cfix_tcp_transport_send(cfix_tcp_transport_t *self, void *data, size_t length)
{
    return (int)(send(self->socket, data, length, 0));
}

int cfix_tcp_transport_recv(cfix_tcp_transport_t *self, void *data, size_t length)
{
    return (int)(recv(self->socket, data, length, 0));
}

void cfix_tcp_transport_free(cfix_tcp_transport_t *self)
{
    CLOSE(self->socket);
    DELETE(self, free);
}
