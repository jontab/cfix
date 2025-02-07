#include "cfix/transport.h"
#include "cfix/memory.h"
#include <stdbool.h>
#include <string.h>

LIST_GENERATE_SOURCE(cfix_char_list, char)

void cfix_transport_send_chunker_add(cfix_transport_send_chunker_t *self, char *buffer, int length);
int  cfix_transport_recv_chunker_populate(cfix_transport_recv_chunker_t *self);
int  cfix_transport_recv_chunker_ready(cfix_transport_recv_chunker_t *self);

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

int cfix_transport_sendall_impl(cfix_transport_t *self, char *buffer, int length)
{
    int total = 0;
    while (total < length)
    {
        int got = cfix_transport_send(self, buffer + total, length - total);
        if (got > 0)
        {
            total += got;
        }
        else
        {
            perror("cfix_transport_send");
            return -1;
        }
    }

    return 0;
}

int cfix_transport_recvall_impl(cfix_transport_t *self, char *buffer, int length)
{
    int total = 0;
    while (total < length)
    {
        int got = cfix_transport_recv(self, buffer + total, length - total);
        if (got > 0)
        {
            total += got;
        }
        else
        {
            perror("cfix_transport_recv");
            return -1;
        }
    }

    return 0;
}

void cfix_transport_send_chunker_init(cfix_transport_send_chunker_t *self, cfix_transport_t *transport)
{
    CLEAR(self);
    self->transport = transport;
}

int cfix_transport_send_chunker_send(cfix_transport_send_chunker_t *self, char *buffer, int length)
{
    if (self->transport == NULL)
    {
        return -1;
    }

    if (length == 0)
    {
        return 0;
    }

    int remaining = sizeof(self->buffer) - self->length;
    if (length < remaining)
    {
        cfix_transport_send_chunker_add(self, buffer, length);
    }
    else
    {
        cfix_transport_send_chunker_add(self, buffer, remaining);

        // Note: We're consuming `remaining` bytes of `buffer`.
        if (cfix_transport_send_chunker_flush(self) < 0)
        {
            return -1;
        }

        if (cfix_transport_send_chunker_send(self, buffer + remaining, length - remaining) < 0)
        {
            return -1;
        }
    }

    return 0;
}

int cfix_transport_send_chunker_flush(cfix_transport_send_chunker_t *self)
{
    if (self->length == 0)
    {
        return 0;
    }

    if (cfix_transport_sendall(self->transport, self->buffer, self->length) < 0)
    {
        return -1;
    }

    self->length = 0;
    return 0;
}

int cfix_transport_recv_chunker_init(cfix_transport_recv_chunker_t *self, cfix_transport_t *transport)
{
    CLEAR(self);
    self->transport = transport;
    return cfix_char_list_reserve(&self->buffer, BUFSIZ);
}

void cfix_transport_recv_chunker_fini(cfix_transport_recv_chunker_t *self)
{
    cfix_char_list_fini(&self->buffer);
    CLEAR(self);
}

int cfix_transport_recv_chunker_recv(cfix_transport_recv_chunker_t *self, char **buffer, int *length)
{
    while (1)
    {
        int message_length = cfix_transport_recv_chunker_ready(self);
        if (message_length < 0)
        {
            if (cfix_transport_recv_chunker_populate(self) < 0)
            {
                perror("cfix_transport_recv_chunker_populate");
                return -1;
            }
            else
            {
                continue;
            }
        }
        else
        {
            *buffer = self->buffer.data;
            *length = message_length;
            return 0;
        }
    }
}

void cfix_transport_recv_chunker_shift(cfix_transport_recv_chunker_t *self, int length)
{
    length = MIN(length, self->buffer.size);
    if (length)
    {
        self->buffer.size -= length;
        memmove(self->buffer.data, &self->buffer.data[length], self->buffer.size);
    }
}

/******************************************************************************/
/* Static                                                                     */
/******************************************************************************/

void cfix_transport_send_chunker_add(cfix_transport_send_chunker_t *self, char *buffer, int length)
{
    // Note: We don't do bounds-checking here.
    memcpy(&self->buffer[self->length], buffer, length);
    self->length += length;
}

int cfix_transport_recv_chunker_populate(cfix_transport_recv_chunker_t *self)
{
    cfix_char_list_reserve(&self->buffer, self->buffer.size + BUFSIZ);
    int got = cfix_transport_recv(self->transport, &self->buffer.data[self->buffer.size], BUFSIZ);
    if (got > 0)
    {
        self->buffer.size += got;
        return 0;
    }
    else
    {
        return -1;
    }
}

int cfix_transport_recv_chunker_ready(cfix_transport_recv_chunker_t *self)
{
    //
    char *checksum_start = memmem(self->buffer.data, self->buffer.size, "\00110=", 4);
    if (!checksum_start)
        return -1;

    //
    int checksum_end_length = self->buffer.size - ((int)(checksum_start - self->buffer.data) + 4);
    if (checksum_end_length < 0)
        return -1;

    //
    char *checksum_end = memmem(checksum_start + 4, checksum_end_length, "\001", 1);
    if (!checksum_end)
        return -1;

    //
    return (int)(checksum_end - self->buffer.data) + 1;
}
