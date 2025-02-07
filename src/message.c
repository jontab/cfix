#include "cfix/message.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int cfix_message_fields_send(cfix_message_fields_t *self, cfix_transport_send_chunker_t *chunker);
int cfix_message_field_send(cfix_message_field_t *self, cfix_transport_send_chunker_t *chunker);

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

int cfix_message_send(cfix_message_t *self, cfix_transport_t *transport)
{
    cfix_transport_send_chunker_t chunker;
    cfix_transport_send_chunker_init(&chunker, transport);
    if ((cfix_message_fields_send(&self->header, &chunker) < 0) ||
        (cfix_message_fields_send(&self->body, &chunker) < 0) ||
        (cfix_message_fields_send(&self->trailer, &chunker) < 0))
    {
        return -1;
    }

    // Note: There might be some residual data left in the `chunker`.
    if (cfix_transport_send_chunker_flush(&chunker) < 0)
    {
        return -1;
    }

    return 0;
}

int cfix_message_recv(cfix_message_t *self, cfix_transport_recv_chunker_t *chunker)
{
    return 0;
}

/******************************************************************************/
/* Static                                                                     */
/******************************************************************************/

int cfix_message_fields_send(cfix_message_fields_t *self, cfix_transport_send_chunker_t *chunker)
{
    for (int i = 0; i < self->size; i++)
        if (cfix_message_field_send(&self->data[i], chunker) < 0)
            return -1;
    return 0;
}

int cfix_message_field_send(cfix_message_field_t *self, cfix_transport_send_chunker_t *chunker)
{
    char tag[8];
    snprintf(tag, sizeof(tag), "%d", self->tag);
    if ((cfix_transport_send_chunker_send(chunker, tag, strlen(tag)) < 0) ||
        (cfix_transport_send_chunker_send(chunker, "=", 1) < 0) ||
        (cfix_transport_send_chunker_send(chunker, cfix_string_data(&self->value), self->value.length) < 0) ||
        (cfix_transport_send_chunker_send(chunker, "\001", 1) < 0))
    {
        return -1;
    }

    return 0;
}
