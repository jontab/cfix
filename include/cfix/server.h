#ifndef CFIX_SERVER_H
#define CFIX_SERVER_H

#include "cfix/transport.h"
#include <stddef.h>

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define cfix_server_start(X) ((cfix_server_t *)(X))->start((cfix_server_t *)(X))
#define cfix_server_stop(X)  ((cfix_server_t *)(X))->stop((cfix_server_t *)(X))

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct cfix_serverargs_s cfix_serverargs_t;
typedef struct cfix_server_s     cfix_server_t;

typedef int (*cfix_server_start_t)(cfix_server_t *self);
typedef void (*cfix_server_stop_t)(cfix_server_t *self);
typedef void (*cfix_server_on_client_t)(cfix_transport_t *transport, void *user_data);

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct cfix_serverargs_s
{
    const char             *host;
    const char             *port;
    cfix_server_on_client_t on_client;
    void                   *on_client_user_data;
};

struct cfix_server_s
{
    cfix_server_start_t start;
    cfix_server_stop_t  stop;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

cfix_server_t *cfix_server_create_default(const cfix_serverargs_t *args);

#endif // CFIX_SERVER_H
