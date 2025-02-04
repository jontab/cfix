#ifndef CFIX_ACCEPTOR_H
#define CFIX_ACCEPTOR_H

#include "cfix/transport.h"
#include <stddef.h>

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define cfix_acceptor_start(X) ((cfix_acceptor_t *)(X))->start((cfix_acceptor_t *)(X))
#define cfix_acceptor_stop(X)  ((cfix_acceptor_t *)(X))->stop((cfix_acceptor_t *)(X))

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct cfix_acceptorargs_s cfix_acceptorargs_t;
typedef struct cfix_acceptor_s     cfix_acceptor_t;

typedef int (*cfix_acceptor_start_t)(cfix_acceptor_t *self);
typedef void (*cfix_acceptor_stop_t)(cfix_acceptor_t *self);
typedef void (*cfix_acceptor_on_transport_t)(cfix_transport_t *transport, void *user_data);

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct cfix_acceptorargs_s
{
    const char                  *host;
    const char                  *port;
    cfix_acceptor_on_transport_t on_transport;
    void                        *on_transport_user_data;
};

struct cfix_acceptor_s
{
    cfix_acceptor_start_t start;
    cfix_acceptor_stop_t  stop;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

cfix_acceptor_t *cfix_acceptor_create_default(const cfix_acceptorargs_t *args);

#endif // CFIX_ACCEPTOR_H
