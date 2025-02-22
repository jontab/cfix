#include "cfix/message.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define SOB '\001'

LIST_GENERATE_SOURCE(cfix_message_fields, cfix_message_field_t)

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct cfix_message_recv_context_s cfix_message_recv_context_t;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct cfix_message_recv_context_s
{
    char                 *field;
    char                 *end;
    cfix_message_field_t *prev;
    TagContext           *context;
};

int   cfix_message_fields_send(cfix_message_fields_t *self, cfix_transport_send_chunker_t *chunker);
int   cfix_message_field_send(cfix_message_field_t *self, cfix_transport_send_chunker_t *chunker);
int   cfix_message_field_recv(int *tag, char **value, cfix_message_recv_context_t *recv);
char *cfix_strnchr(char *cursor, char *end, char ch);

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

int cfix_message_from_string(cfix_message_t *self, char *text, int length, TagContext *context)
{
    cfix_message_recv_context_t recv = {
        .field = text,
        .end = text + length,
        .prev = NULL,
        .context = context,
    };
    while (recv.field < recv.end)
    {
        int   result;
        int   tag;
        char *value;
        if ((result = cfix_message_field_recv(&tag, &value, &recv)) < 0)
        {
            return result;
        }

        //
        cfix_message_fields_t *section;
        if (context->is_header_tag(context, tag))
        {
            section = &self->header;
        }
        else if (context->is_trailer_tag(context, tag))
        {
            section = &self->trailer;
        }
        else
        {
            section = &self->body;
        }

        if (cfix_message_fields_add(section, tag, value))
        {
            return -1;
        }

        recv.prev = &section->data[section->size - 1];
    }

    return 0;
}

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

int cfix_message_recv(cfix_message_t *self, cfix_transport_recv_chunker_t *chunker, TagContext *context)
{
    char *msg_start;
    int   msg_length;
    if (cfix_transport_recv_chunker_recv(chunker, &msg_start, &msg_length) < 0)
    {
        return -1;
    }

    int result = cfix_message_from_string(self, msg_start, msg_length, context);
    if (result < 0)
    {
        return result;
    }

    cfix_transport_recv_chunker_shift(chunker, msg_length);
    return 0;
}

void cfix_message_fini(cfix_message_t *self)
{
    cfix_message_fields_fini(&self->header);
    cfix_message_fields_fini(&self->body);
    cfix_message_fields_fini(&self->trailer);
}

/******************************************************************************/
/* Static                                                                     */
/******************************************************************************/

int cfix_message_fields_add(cfix_message_fields_t *self, int tag, const char *text)
{
    if ((cfix_message_fields_append(self, (cfix_message_field_t){.tag = tag}) == 0) &&
        (cfix_string_init(&self->data[self->size - 1].value, text) == 0))
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

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

int cfix_message_field_recv(int *field_tag, char **field_value, cfix_message_recv_context_t *recv)
{
    char *equals = cfix_strnchr(recv->field, recv->end, '=');
    if (equals == recv->end)
    {
        return -1;
    }

    char *sob;
    char *value = equals + 1;
    if (value == recv->end)
    {
        return -2;
    }

    *equals = '\0';
    if (recv->context->is_data_tag(recv->context, atoi(recv->field)))
    {
        if (!recv->prev)
        {
            return -3;
        }

        char *prev_value = cfix_string_data(&recv->prev->value);
        int   prev_value_length = atoi(prev_value);
        sob = value + prev_value_length;
    }
    else
    {
        sob = cfix_strnchr(value, recv->end, SOB);
    }

    // We should always land on '\001'. If we don't, then it's malformed.
    if ((sob >= recv->end) || (*sob != SOB))
    {
        return -4;
    }

    *sob = '\0';
    *field_tag = atoi(recv->field); // Result.
    *field_value = value;           // Result.
    recv->field = sob + 1;
    return 0;
}

char *cfix_strnchr(char *cursor, char *end, char ch)
{
    for (char *p = cursor; p < end; p++)
        if (*p == ch)
            return p;
    return end;
}
