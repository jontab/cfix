#include "cfix/parser.h"
#include <munit.h>
#include <stdio.h> // snprintf
#include <unistd.h>

void        write_string_to_ring(cfix_ring_t *ring, const char *text);
void        unit_parser_expect(const char *text, int result);
MunitResult unit_parser_begin_string_not_yet(const MunitParameter params[], void *data);
MunitResult unit_parser_begin_string_not_match(const MunitParameter params[], void *data);
MunitResult unit_parser_body_length_not_yet(const MunitParameter params[], void *data);
MunitResult unit_parser_body_length_invalid(const MunitParameter params[], void *data);
MunitResult unit_parser_checksum_not_yet(const MunitParameter params[], void *data);
MunitResult unit_parser_checksum_invalid(const MunitParameter params[], void *data);
MunitResult unit_parser_checksum_correct(const MunitParameter params[], void *data);

MunitTest unit_parser_tests[] = {
    {"/begin_string_not_yet", unit_parser_begin_string_not_yet, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/begin_string_not_match", unit_parser_begin_string_not_match, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/body_length_not_yet", unit_parser_body_length_not_yet, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/body_length_invalid", unit_parser_body_length_invalid, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/checksum_not_yet", unit_parser_checksum_not_yet, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/checksum_invalid", unit_parser_checksum_invalid, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/checksum_correct", unit_parser_checksum_correct, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

MunitResult unit_parser_begin_string_not_yet(const MunitParameter params[], void *data)
{
    unit_parser_expect("", CFIX_INCOMPLETE);
    unit_parser_expect("8", CFIX_INCOMPLETE);
    unit_parser_expect("8=", CFIX_INCOMPLETE);
    unit_parser_expect("8=F", CFIX_INCOMPLETE);
    unit_parser_expect("8=FI", CFIX_INCOMPLETE);
    unit_parser_expect("8=FIX", CFIX_INCOMPLETE);
    unit_parser_expect("8=FIX.", CFIX_INCOMPLETE);
    unit_parser_expect("8=FIX.4", CFIX_INCOMPLETE);
    unit_parser_expect("8=FIX.4.", CFIX_INCOMPLETE);
    unit_parser_expect("8=FIX.4.1", CFIX_INCOMPLETE);
    return MUNIT_OK;
}

MunitResult unit_parser_begin_string_not_match(const MunitParameter params[], void *data)
{
    unit_parser_expect("8=FIX.4.1\0019", CFIX_VALIDATE_BEGIN_STRING);
    unit_parser_expect("8=FIX.4.2\0019", CFIX_VALIDATE_BEGIN_STRING);
    unit_parser_expect("8=FIX.4.3\0019", CFIX_VALIDATE_BEGIN_STRING);
    return MUNIT_OK;
}

MunitResult unit_parser_body_length_not_yet(const MunitParameter params[], void *data)
{
    unit_parser_expect("8=FIX.4.0\001", CFIX_INCOMPLETE);
    unit_parser_expect("8=FIX.4.0\0019", CFIX_INCOMPLETE);
    unit_parser_expect("8=FIX.4.0\0019=", CFIX_INCOMPLETE);
    unit_parser_expect("8=FIX.4.0\0019=1", CFIX_INCOMPLETE);
    unit_parser_expect("8=FIX.4.0\0019=11", CFIX_INCOMPLETE);
    return MUNIT_OK;
}

MunitResult unit_parser_body_length_invalid(const MunitParameter params[], void *data)
{
    unit_parser_expect("8=FIX.4.0\0019=a", CFIX_INVALID_BODY_LENGTH);
    unit_parser_expect("8=FIX.4.0\0019=1a", CFIX_INVALID_BODY_LENGTH);
    unit_parser_expect("8=FIX.4.0\0019=11a", CFIX_INVALID_BODY_LENGTH);
    return MUNIT_OK;
}

MunitResult unit_parser_checksum_not_yet(const MunitParameter params[], void *data)
{
    unit_parser_expect("8=FIX.4.0\0019=0\00110=1", CFIX_INCOMPLETE);
    unit_parser_expect("8=FIX.4.0\0019=0\00110=11", CFIX_INCOMPLETE);
    unit_parser_expect("8=FIX.4.0\0019=0\00110=111", CFIX_INCOMPLETE);
    return MUNIT_OK;
}

MunitResult unit_parser_checksum_invalid(const MunitParameter params[], void *data)
{
    unit_parser_expect("8=FIX.4.0\0019=0\00110=11a\001", CFIX_VALIDATE_CHECKSUM);
    unit_parser_expect("8=FIX.4.0\0019=0\00110=1a1\001", CFIX_VALIDATE_CHECKSUM);
    unit_parser_expect("8=FIX.4.0\0019=0\00110=a11\001", CFIX_VALIDATE_CHECKSUM);
    unit_parser_expect("8=FIX.4.0\0019=0\00110=000\001", CFIX_VALIDATE_CHECKSUM);
    unit_parser_expect("8=FIX.4.0\0019=0\00110=001\001", CFIX_VALIDATE_CHECKSUM);
    return MUNIT_OK;
}

MunitResult unit_parser_checksum_correct(const MunitParameter params[], void *data)
{
    unit_parser_expect("8=FIX.4.0\0019=0\00110=196\001", 0);
    return MUNIT_OK;
}

void write_string_to_ring(cfix_ring_t *ring, const char *text)
{
    size_t available;
    char  *write = cfix_ring_reserve(ring, &available);
    munit_assert_size(strlen(text), <=, available);
    strncpy(write, text, strlen(text));
    cfix_ring_commit(ring, strlen(text));
}

void unit_parser_expect(const char *text, int result)
{
    munit_logf(MUNIT_LOG_INFO, "Text: %s", text);

    cfix_ring_t ring;
    munit_assert_int(cfix_ring_init(&ring, getpagesize()), ==, 0);
    write_string_to_ring(&ring, text);

    cfix_parser_config_t config = {
        .begin_string = "FIX.4.0",
        .validate_begin_string = 1,
        .validate_checksum = 1,
        .separator_field = '\001',
    };
    cfix_parser_t parser;
    cfix_parser_init(&parser, &config);

    // Execute.
    munit_assert_int(cfix_parser_get(&parser, &ring, NULL), ==, result);

    // Cleanup.
    cfix_ring_fini(&ring);
}
