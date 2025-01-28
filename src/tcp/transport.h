#ifndef CFIX_TCP_TRANSPORT_H
#define CFIX_TCP_TRANSPORT_H

#include "cfix/transport.h"

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct cfix_tcp_transport_s cfix_tcp_transport_t;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct cfix_tcp_transport_s
{
    cfix_transport_t base;
    int              socket;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

cfix_tcp_transport_t *cfix_tcp_transport_create(int socket);

#endif // CFIX_TCP_TRANSPORT_H
