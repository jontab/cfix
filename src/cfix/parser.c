#include "cfix/parser.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
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
int cfix_parser_handle_body(cfix_parser_t *self, get_context_t *ctx, cfix_message_t *out);

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

    out->bytes = context.checksum_end - context.begin_string_tag;

    *(context.body_length_tag - 1) = '\0';
    *(context.body_start - 1) = '\0';
    *(context.checksum_end - 1) = '\0';

#if 0
    fprintf(stdout, "bytes:        %zu\n", out->bytes);
    fprintf(stdout, "ctx.bs_tag:   '%s'\n", context.begin_string_tag);
    fprintf(stdout, "ctx.bs_value: '%s'\n", context.begin_string_value);
    fprintf(stdout, "ctx.bl_tag:   '%s'\n", context.body_length_tag);
    fprintf(stdout, "ctx.bl_value: '%s'\n", context.body_length_value);
    fprintf(stdout, "ctx.bs:       '%s'\n", context.body_start);
    fprintf(stdout, "ctx.ck_tag:   '%s'\n", context.checksum_tag);
    fprintf(stdout, "ctx.ck_value: '%s'\n", context.checksum_value);
#endif

    if ((cfix_message_field_list_append(&out->header, 8, context.begin_string_value) < 0) ||
        (cfix_message_field_list_append(&out->header, 9, context.body_length_value) < 0))
    {
    clean:
        cfix_message_field_list_clear(&out->header);
        cfix_message_field_list_clear(&out->body);
        cfix_message_field_list_clear(&out->trailer);
        cfix_ring_consume(buffer, out->bytes);
        return CFIX_MEMORY;
    }

    if ((result = cfix_parser_handle_body(self, &context, out)) < 0)
    {
        return result;
    }

    if ((cfix_message_field_list_append(&out->trailer, 10, context.checksum_value) < 0))
    {
        goto clean;
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

int cfix_parser_handle_body(cfix_parser_t *self, get_context_t *ctx, cfix_message_t *out)
{
    char *last_value = NULL;
    char *p = ctx->body_start;
    while (p < ctx->checksum_tag)
    {
        char *equals = memchr(p, self->config.separator_tag_value, ctx->checksum_tag - p);
        if (!equals)
        {
            return CFIX_INVALID_FIELD;
        }

        *equals = '\0';
        int tag = atoi(p);
        if (tag >= self->config.tag_table->size)
        {
            fprintf(stdout, "error: unknown tag: %d\n", tag);
            return CFIX_INVALID_FIELD;
        }

        char *value = equals + 1;
        char *soh;
        if (self->config.tag_table->data[tag].is_data_type)
        {
            if (!last_value)
            {
                return CFIX_INVALID_FIELD;
            }
            
            soh = value + atoi(last_value);
        }
        else
        {
            soh = memchr(value, self->config.separator_field, ctx->checksum_tag - value);
        }
        
        if (!soh || (soh >= ctx->checksum_tag))
        {
            return CFIX_INVALID_FIELD;
        }

        cfix_message_field_list_t *section;
        if (self->config.tag_table->data[tag].is_header_field)
        {
            section = &out->header;
        }
        else if (self->config.tag_table->data[tag].is_trailer_field)
        {
            section = &out->trailer;
        }
        else
        {
            section = &out->body;
        }

        *soh = '\0';
        if (cfix_message_field_list_append(section, tag, value) < 0)
        {
            return CFIX_MEMORY;
        }

        last_value = value;
        p = soh + 1;
    }

    return 0;
}
