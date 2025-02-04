#include "./tcp/initiator.h"

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

cfix_initiator_t *cfix_initiator_create_default(const cfix_initiatorargs_t *args)
{
    return cfix_tcp_initiator_create(args);
}
