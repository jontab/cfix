#include "./tcp/server.h"

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

cfix_server_t *cfix_server_create_default(const cfix_serverargs_t *args)
{
    return cfix_tcp_server_create(args);
}
