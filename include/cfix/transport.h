#ifndef CFIX_TRANSPORT_H
#define CFIX_TRANSPORT_H

#include <stddef.h>

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define cfix_transport_send(X, DATA, LEN) ((cfix_transport_t *)(X))->send((cfix_transport_t *)(X), (DATA), (LEN))
#define cfix_transport_recv(X, DATA, LEN) ((cfix_transport_t *)(X))->recv((cfix_transport_t *)(X), (DATA), (LEN))
#define cfix_transport_free(X)            ((cfix_transport_t *)(X))->free((cfix_transport_t *)(X))

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct cfix_transport_s cfix_transport_t;

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

#endif // CFIX_TRANSPORT_H
