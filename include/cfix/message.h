#ifndef CFIX_MESSAGE_H
#define CFIX_MESSAGE_H

#include "cfix/list.h"
#include "cfix/string.h"

struct cfix_message_field_s;

LIST_GENERATE_HEADER(cfix_message_fields, struct cfix_message_field)

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

#endif // CFIX_MESSAGE_H
