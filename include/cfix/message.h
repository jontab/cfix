#ifndef CFIX_MESSAGE_H
#define CFIX_MESSAGE_H

#include "cfix/list.h"
#include "cfix/string.h"
#include "cfix/transport.h"

struct cfix_message_field_s;

LIST_GENERATE_HEADER(cfix_message_fields, struct cfix_message_field_s)

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct cfix_message_field_s cfix_message_field_t;
typedef struct cfix_message_s       cfix_message_t;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct cfix_message_field_s
{
    int           tag;
    cfix_string_t value;
};

struct cfix_message_s
{
    cfix_message_fields_t header;
    cfix_message_fields_t body;
    cfix_message_fields_t trailer;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

int cfix_message_send(cfix_message_t *self, cfix_transport_t *transport);
int cfix_message_recv(cfix_message_t *self, cfix_transport_recv_chunker_t *chunker);

#endif // CFIX_MESSAGE_H
