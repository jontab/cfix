#ifndef CFIX_MESSAGE_H
#define CFIX_MESSAGE_H

#include <stddef.h>

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct cfix_message_field_s      cfix_message_field_t;
typedef struct cfix_message_field_list_s cfix_message_field_list_t;
typedef struct cfix_message_s            cfix_message_t;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct cfix_message_field_s
{
    int   tag;
    char *value;
};

struct cfix_message_field_list_s
{
    cfix_message_field_t *data;
    size_t                size;
    size_t                capacity;
};

struct cfix_message_s
{
    cfix_message_field_list_t header;
    cfix_message_field_list_t body;
    cfix_message_field_list_t trailer;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void cfix_message_field_list_init(cfix_message_field_list_t *self);
void cfix_message_field_list_fini(cfix_message_field_list_t *self);
int  cfix_message_field_list_reserve(cfix_message_field_list_t *self, size_t capacity);
int  cfix_message_field_list_append(cfix_message_field_list_t *self, int tag, char *value);
void cfix_message_field_list_clear(cfix_message_field_list_t *self);
void cfix_message_init(cfix_message_t *self);
void cfix_message_fini(cfix_message_t *self);

#endif // CFIX_MESSAGE_H
