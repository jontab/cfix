#ifndef CFIX_INITIATOR_H
#define CFIX_INITIATOR_H

#include "cfix/transport.h"
#include <stddef.h>

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define cfix_initiator_start(X) ((cfix_initiator_t *)(X))->start((cfix_initiator_t *)(X))
#define cfix_initiator_stop(X)  ((cfix_initiator_t *)(X))->stop((cfix_initiator_t *)(X))

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct cfix_initiatorargs_s cfix_initiatorargs_t;
typedef struct cfix_initiator_s     cfix_initiator_t;

typedef int (*cfix_initiator_start_t)(cfix_initiator_t *self);
typedef void (*cfix_initiator_stop_t)(cfix_initiator_t *self);
typedef void (*cfix_initiator_on_transport_t)(cfix_transport_t *transport, void *user_data);

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct cfix_initiatorargs_s
{
    const char                   *host;
    const char                   *port;
    cfix_initiator_on_transport_t on_transport;
    void                         *on_transport_user_data;
};

struct cfix_initiator_s
{
    cfix_initiator_start_t start;
    cfix_initiator_stop_t  stop;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

cfix_initiator_t *cfix_initiator_create_default(const cfix_initiatorargs_t *args);

#endif // CFIX_INITIATOR_H
