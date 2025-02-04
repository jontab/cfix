#include "./tcp/acceptor.h"

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

cfix_acceptor_t *cfix_acceptor_create_default(const cfix_acceptorargs_t *args)
{
    return cfix_tcp_acceptor_create(args);
}
