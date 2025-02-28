#include "cfix/message.h"
#include <stdlib.h>
#include <string.h>

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void cfix_message_field_list_init(cfix_message_field_list_t *self)
{
    memset(self, 0, sizeof(cfix_message_field_list_t));
}

void cfix_message_field_list_fini(cfix_message_field_list_t *self)
{
    cfix_message_field_list_clear(self);
    free(self->data);
}

int cfix_message_field_list_reserve(cfix_message_field_list_t *self, size_t capacity)
{
    if (self->capacity < capacity)
    {
        size_t new_capacity = 2 * capacity + 7;
        void  *new_data = realloc(self->data, new_capacity * sizeof(cfix_message_field_t));
        if (new_data)
        {
            self->capacity = new_capacity;
            self->data = new_data;
        }
        else
        {
            return -1;
        }
    }

    return 0;
}

int cfix_message_field_list_append(cfix_message_field_list_t *self, int tag, char *value)
{
    if (cfix_message_field_list_reserve(self, self->size + 1) == 0)
    {
        self->data[self->size].tag = tag;
        self->data[self->size].value = value;
        self->size++;
        return 0;
    }
    else
    {
        return -1;
    }
}

void cfix_message_field_list_clear(cfix_message_field_list_t *self)
{
    self->size = 0;
}

void cfix_message_init(cfix_message_t *self)
{
    cfix_message_field_list_init(&self->header);
    cfix_message_field_list_init(&self->body);
    cfix_message_field_list_init(&self->trailer);
}

void cfix_message_fini(cfix_message_t *self)
{
    cfix_message_field_list_fini(&self->header);
    cfix_message_field_list_fini(&self->body);
    cfix_message_field_list_fini(&self->trailer);
}
