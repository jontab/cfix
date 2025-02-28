#ifndef CFIX_PARSER_H
#define CFIX_PARSER_H

#include <cfix/message.h>
#include <cfix/ring.h>
#include <stdbool.h>
#include <stdint.h>

#define CFIX_INCOMPLETE            -1
#define CFIX_VALIDATE_BEGIN_STRING -2
#define CFIX_INVALID_BODY_LENGTH   -3
#define CFIX_VALIDATE_CHECKSUM     -4
#define CFIX_MEMORY                -5
#define CFIX_INVALID_FIELD         -6

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct cfix_tag_entry_s     cfix_tag_entry_t;
typedef struct cfix_tag_table_s     cfix_tag_table_t;
typedef struct cfix_parser_config_s cfix_parser_config_t;
typedef struct cfix_parser_s        cfix_parser_t;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct cfix_tag_entry_s
{
    uint8_t is_data_type : 1;
    uint8_t is_header_field : 1;
    uint8_t is_trailer_field : 1;
};

struct cfix_tag_table_s
{
    cfix_tag_entry_t *data;
    size_t            size;
};

struct cfix_parser_config_s
{
    cfix_tag_table_t *tag_table;
    char              separator_field;
    char              separator_tag_value;
    char              separator_message;
    bool              validate_begin_string;
    bool              validate_checksum;
    const char       *begin_string;
};

struct cfix_parser_s
{
    cfix_parser_config_t config;
    size_t               begin_string_length;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void cfix_parser_init(cfix_parser_t *self, const cfix_parser_config_t *config);
int  cfix_parser_get(cfix_parser_t *self, cfix_ring_t *buffer, cfix_message_t *out);

#endif // CFIX_PARSER_H
