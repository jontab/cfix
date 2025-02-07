#ifndef CFIX_TRANSPORT_H
#define CFIX_TRANSPORT_H

#include "cfix/list.h"
#include <stddef.h>
#include <stdio.h>

LIST_GENERATE_HEADER(cfix_char_list, char)

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define cfix_transport_sendall(X, DATA, LEN) cfix_transport_sendall_impl((cfix_transport_t *)(X), (DATA), (LEN))
#define cfix_transport_recvall(X, DATA, LEN) cfix_transport_recvall_impl((cfix_transport_t *)(X), (DATA), (LEN))
#define cfix_transport_send(X, DATA, LEN)    ((cfix_transport_t *)(X))->send((cfix_transport_t *)(X), (DATA), (LEN))
#define cfix_transport_recv(X, DATA, LEN)    ((cfix_transport_t *)(X))->recv((cfix_transport_t *)(X), (DATA), (LEN))
#define cfix_transport_free(X)               ((cfix_transport_t *)(X))->free((cfix_transport_t *)(X))

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct cfix_transport_s              cfix_transport_t;
typedef struct cfix_transport_send_chunker_s cfix_transport_send_chunker_t;
typedef struct cfix_transport_recv_chunker_s cfix_transport_recv_chunker_t;

typedef int (*cfix_transport_send_t)(cfix_transport_t *self, void *data, size_t length);
typedef int (*cfix_transport_recv_t)(cfix_transport_t *self, void *data, size_t length);
typedef void (*cfix_transport_free_t)(cfix_transport_t *self);

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct cfix_transport_s
{
    cfix_transport_send_t send;
    cfix_transport_recv_t recv;
    cfix_transport_free_t free;
};

/**
 * @brief This is essentially our own Nagle's algorithm. We use it in specific scenarios.
 */
struct cfix_transport_send_chunker_s
{
    cfix_transport_t *transport;
    char              buffer[BUFSIZ];
    int               length;
};

struct cfix_transport_recv_chunker_s
{
    cfix_transport_t *transport;
    cfix_char_list_t  buffer;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

int  cfix_transport_sendall_impl(cfix_transport_t *self, char *buffer, int length);
int  cfix_transport_recvall_impl(cfix_transport_t *self, char *buffer, int length);
void cfix_transport_send_chunker_init(cfix_transport_send_chunker_t *self, cfix_transport_t *transport);
int  cfix_transport_send_chunker_send(cfix_transport_send_chunker_t *self, char *buffer, int length);
int  cfix_transport_send_chunker_flush(cfix_transport_send_chunker_t *self);
int  cfix_transport_recv_chunker_init(cfix_transport_recv_chunker_t *self, cfix_transport_t *transport);
void cfix_transport_recv_chunker_fini(cfix_transport_recv_chunker_t *self);
int  cfix_transport_recv_chunker_recv(cfix_transport_recv_chunker_t *self, char **buffer, int *length);
void cfix_transport_recv_chunker_shift(cfix_transport_recv_chunker_t *self, int length);

#endif // CFIX_TRANSPORT_H
