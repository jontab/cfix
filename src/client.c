#include "./tcp/client.h"

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

cfix_client_t *cfix_client_create_default(const cfix_clientargs_t *args)
{
    return cfix_tcp_client_create(args);
}
