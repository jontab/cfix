#include "cfix/parser.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct get_context_s get_context_t;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct get_context_s
{
    char  *begin_string_tag;
    char  *begin_string_value;
    char  *body_length_tag;
    char  *body_length_value;
    char  *body_start;
    size_t body_length;
    char  *checksum_tag;
    char  *checksum_value;
    char  *checksum_end;
};

int cfix_parser_handle_begin_string(cfix_parser_t *self, char *data_end, get_context_t *ctx);
int cfix_parser_handle_body_length(cfix_parser_t *self, char *data_end, get_context_t *ctx);
int cfix_parser_handle_checksum(cfix_parser_t *self, char *data_end, get_context_t *ctx);

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void cfix_parser_init(cfix_parser_t *self, const cfix_parser_config_t *config)
{
    memcpy(&self->config, config, sizeof(self->config));
    self->begin_string_length = strlen(config->begin_string);
}

int cfix_parser_get(cfix_parser_t *self, cfix_ring_t *buffer, cfix_message_t *out)
{
    size_t        size;
    get_context_t context;
    int           result;
    context.begin_string_tag = cfix_ring_read(buffer, &size);                             // "8=".
    context.begin_string_value = context.begin_string_tag + 2;                            // "8=".
    context.body_length_tag = context.begin_string_value + self->begin_string_length + 1; // "8=FIX.4.0\001".
    context.body_length_value = context.body_length_tag + 2;                              // "9=".
    if (((result = cfix_parser_handle_begin_string(self, context.begin_string_tag + size, &context)) < 0) ||
        ((result = cfix_parser_handle_body_length(self, context.begin_string_tag + size, &context)) < 0) ||
        ((result = cfix_parser_handle_checksum(self, context.begin_string_tag + size, &context)) < 0))
    {
        return result;
    }

    return 0;
}

int cfix_parser_handle_begin_string(cfix_parser_t *self, char *data_end, get_context_t *ctx)
{
    if (ctx->body_length_tag < data_end)
    {
        if (self->config.validate_begin_string)
        {
            if (memcmp(ctx->begin_string_value, self->config.begin_string, self->begin_string_length))
            {
                return CFIX_VALIDATE_BEGIN_STRING;
            }
        }

        return 0;
    }
    else
    {
        return CFIX_INCOMPLETE;
    }
}

int cfix_parser_handle_body_length(cfix_parser_t *self, char *data_end, get_context_t *ctx)
{
    if (ctx->body_length_value < data_end)
    {
        ctx->body_length = 0;
        char *p = ctx->body_length_value;
        while (1)
        {
            if (p == data_end)
            {
                return CFIX_INCOMPLETE;
            }

            if (*p == self->config.separator_field)
            {
                break;
            }

            if (!isdigit(*p))
            {
                return CFIX_INVALID_BODY_LENGTH;
            }

            ctx->body_length = ctx->body_length * 10 + (*p - '0');
            p++;
        }

        ctx->body_start = p + 1;
        ctx->checksum_tag = ctx->body_start + ctx->body_length;
        ctx->checksum_value = ctx->checksum_tag + 3; // "10=".
        ctx->checksum_end = ctx->checksum_value + 4; // "XXX\001".
        return 0;
    }
    else
    {
        return CFIX_INCOMPLETE;
    }
}

int cfix_parser_handle_checksum(cfix_parser_t *self, char *data_end, get_context_t *ctx)
{
    if (ctx->checksum_end <= data_end)
    {
        if (self->config.validate_checksum)
        {
            char a = ctx->checksum_value[0];
            char b = ctx->checksum_value[1];
            char c = ctx->checksum_value[2];
            if (!isdigit(a) || !isdigit(b) || !isdigit(c))
            {
                return CFIX_VALIDATE_CHECKSUM;
            }

            int checksum_expected = 0;
            for (char *p = ctx->begin_string_tag; p < ctx->checksum_tag; p++)
            {
                checksum_expected = (checksum_expected + (*p)) % 256;
            }

            int checksum = (a - '0') * 100 + (b - '0') * 10 + (c - '0');
            if (checksum == checksum_expected)
            {
                return 0;
            }
            else
            {
                fprintf(stdout, "error: checksum mismatch: expected %03d, got %03d\n", checksum_expected, checksum);
                return CFIX_VALIDATE_CHECKSUM;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return CFIX_INCOMPLETE;
    }
}
