#ifndef CFIX_CLIENT_H
#define CFIX_CLIENT_H

#include "./transport.h"
#include <stddef.h>

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define cfix_client_start(X) ((cfix_client_t *)(X))->start((cfix_client_t *)(X))
#define cfix_client_stop(X)  ((cfix_client_t *)(X))->stop((cfix_client_t *)(X))

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct cfix_clientargs_s cfix_clientargs_t;
typedef struct cfix_client_s     cfix_client_t;

typedef int (*cfix_client_start_t)(cfix_client_t *self);
typedef void (*cfix_client_stop_t)(cfix_client_t *self);
typedef void (*cfix_client_on_client_t)(cfix_transport_t *transport, void *user_data);

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct cfix_clientargs_s
{
    const char             *host;
    const char             *port;
    cfix_client_on_client_t on_client;
    void                   *on_client_user_data;
};

struct cfix_client_s
{
    cfix_client_start_t start;
    cfix_client_stop_t  stop;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

cfix_client_t *cfix_client_create_default(const cfix_clientargs_t *args);

#endif // CFIX_CLIENT_H
