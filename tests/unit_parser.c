#include "cfix/parser.h"
#include <munit.h>
#include <stdio.h> // snprintf
#include <unistd.h>

void        write_string_to_ring(cfix_ring_t *ring, const char *text);
void        unit_parser_expect(const char *text, int result);
void        unit_parser_parse(const char *text, int result, cfix_ring_t *ring, cfix_message_t *out);
MunitResult unit_parser_begin_string_not_yet(const MunitParameter params[], void *data);
MunitResult unit_parser_begin_string_not_match(const MunitParameter params[], void *data);
MunitResult unit_parser_body_length_not_yet(const MunitParameter params[], void *data);
MunitResult unit_parser_body_length_invalid(const MunitParameter params[], void *data);
MunitResult unit_parser_checksum_not_yet(const MunitParameter params[], void *data);
MunitResult unit_parser_checksum_invalid(const MunitParameter params[], void *data);
MunitResult unit_parser_checksum_correct(const MunitParameter params[], void *data);
MunitResult unit_parser_body_no_equals(const MunitParameter params[], void *data);
MunitResult unit_parser_unknown_tag(const MunitParameter params[], void *data);
MunitResult unit_parser_data_no_last_value(const MunitParameter params[], void *data);
MunitResult unit_parser_message_0(const MunitParameter params[], void *data);
MunitResult unit_parser_message_1(const MunitParameter params[], void *data);
MunitResult unit_parser_message_2_raw(const MunitParameter params[], void *data);

MunitTest unit_parser_tests[] = {
    {"/begin_string_not_yet", unit_parser_begin_string_not_yet, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/begin_string_not_match", unit_parser_begin_string_not_match, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/body_length_not_yet", unit_parser_body_length_not_yet, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/body_length_invalid", unit_parser_body_length_invalid, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/checksum_not_yet", unit_parser_checksum_not_yet, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/checksum_invalid", unit_parser_checksum_invalid, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/checksum_correct", unit_parser_checksum_correct, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/body_no_equals", unit_parser_body_no_equals, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/unknown_tag", unit_parser_unknown_tag, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/data_no_last_value", unit_parser_data_no_last_value, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/message_0", unit_parser_message_0, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/message_1", unit_parser_message_1, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/message_2_raw", unit_parser_message_2_raw, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
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

MunitResult unit_parser_body_no_equals(const MunitParameter params[], void *data)
{
    unit_parser_expect("8=FIX.4.0\0019=2\0011\00110=248\001", CFIX_INVALID_FIELD);
    unit_parser_expect("8=FIX.4.0\0019=3\00111\00110=042\001", CFIX_INVALID_FIELD);
    unit_parser_expect("8=FIX.4.0\0019=4\001111\00110=092\001", CFIX_INVALID_FIELD);
    return MUNIT_OK;
}

MunitResult unit_parser_unknown_tag(const MunitParameter params[], void *data)
{
    unit_parser_expect("8=FIX.4.0\0019=5\001999=\00110=178\001", CFIX_INVALID_FIELD);
    unit_parser_expect("8=FIX.4.0\0019=6\0019999=\00110=236\001", CFIX_INVALID_FIELD);
    unit_parser_expect("8=FIX.4.0\0019=7\00199999=\00110=038\001", CFIX_INVALID_FIELD);
    return MUNIT_OK;
}

MunitResult unit_parser_data_no_last_value(const MunitParameter params[], void *data)
{
    unit_parser_expect("8=FIX.4.0\0019=4\00196=\00110=117\001", CFIX_INVALID_FIELD);

    // Also, check for `RawDataLength` that propels us too far.
    unit_parser_expect("8=FIX.4.0\0019=14\00195=99\00196=data\00110=094\001", CFIX_INVALID_FIELD);
    unit_parser_expect("8=FIX.4.0\0019=15\00195=999\00196=data\00110=152\001", CFIX_INVALID_FIELD);
    unit_parser_expect("8=FIX.4.0\0019=16\00195=9999\00196=data\00110=210\001", CFIX_INVALID_FIELD);
    return MUNIT_OK;
}

MunitResult unit_parser_message_0(const MunitParameter params[], void *data)
{
    cfix_ring_t    ring;
    cfix_message_t msg;
    unit_parser_parse("8=FIX.4.0\0019=0\00110=196\001", 0, &ring, &msg);

    munit_assert_size(msg.header.size, ==, 2);
    munit_assert_size(msg.body.size, ==, 0);
    munit_assert_size(msg.trailer.size, ==, 1);

    munit_assert_int(msg.header.data[0].tag, ==, 8);
    munit_assert_int(msg.header.data[1].tag, ==, 9);
    munit_assert_int(msg.trailer.data[0].tag, ==, 10);

    munit_assert_string_equal(msg.header.data[0].value, "FIX.4.0");
    munit_assert_string_equal(msg.header.data[1].value, "0");
    munit_assert_string_equal(msg.trailer.data[0].value, "196");

    cfix_message_fini(&msg);
    cfix_ring_fini(&ring);
    return MUNIT_OK;
}

MunitResult unit_parser_message_1(const MunitParameter params[], void *data)
{
    cfix_ring_t    ring;
    cfix_message_t msg;
    const char    *text =
        "8=FIX.4.0\0019=72\00135=A\00134=978\00149=SellSide\00152=20190206-16:29:19.208\00156=BuySide\00198=0\001108="
        "60\00110=073\001";
    unit_parser_parse(text, 0, &ring, &msg);

    munit_assert_size(msg.header.size, ==, 7);
    munit_assert_size(msg.body.size, ==, 2);
    munit_assert_size(msg.trailer.size, ==, 1);

    munit_assert_int(msg.header.data[0].tag, ==, 8);
    munit_assert_int(msg.header.data[1].tag, ==, 9);
    munit_assert_int(msg.header.data[2].tag, ==, 35);
    munit_assert_int(msg.header.data[3].tag, ==, 34);
    munit_assert_int(msg.header.data[4].tag, ==, 49);
    munit_assert_int(msg.header.data[5].tag, ==, 52);
    munit_assert_int(msg.header.data[6].tag, ==, 56);
    munit_assert_int(msg.body.data[0].tag, ==, 98);
    munit_assert_int(msg.body.data[1].tag, ==, 108);
    munit_assert_int(msg.trailer.data[0].tag, ==, 10);

    munit_assert_string_equal(msg.header.data[0].value, "FIX.4.0");
    munit_assert_string_equal(msg.header.data[1].value, "72");
    munit_assert_string_equal(msg.header.data[2].value, "A");
    munit_assert_string_equal(msg.header.data[3].value, "978");
    munit_assert_string_equal(msg.header.data[4].value, "SellSide");
    munit_assert_string_equal(msg.header.data[5].value, "20190206-16:29:19.208");
    munit_assert_string_equal(msg.header.data[6].value, "BuySide");
    munit_assert_string_equal(msg.body.data[0].value, "0");
    munit_assert_string_equal(msg.body.data[1].value, "60");
    munit_assert_string_equal(msg.trailer.data[0].value, "073");

    cfix_message_fini(&msg);
    cfix_ring_fini(&ring);
    return MUNIT_OK;
}

MunitResult unit_parser_message_2_raw(const MunitParameter params[], void *data)
{
    cfix_ring_t    ring;
    cfix_message_t msg;
    const char    *text = "8=FIX.4.0\0019=30\00195=20\00196=raw\001data\00110=is\001weird\00110=046\001";
    unit_parser_parse(text, 0, &ring, &msg);

    munit_assert_size(msg.header.size, ==, 2);
    munit_assert_size(msg.body.size, ==, 2);
    munit_assert_size(msg.trailer.size, ==, 1);

    munit_assert_int(msg.header.data[0].tag, ==, 8);
    munit_assert_int(msg.header.data[1].tag, ==, 9);
    munit_assert_int(msg.body.data[0].tag, ==, 95);
    munit_assert_int(msg.body.data[1].tag, ==, 96);
    munit_assert_int(msg.trailer.data[0].tag, ==, 10);

    munit_assert_string_equal(msg.header.data[0].value, "FIX.4.0");
    munit_assert_string_equal(msg.header.data[1].value, "30");
    munit_assert_string_equal(msg.body.data[0].value, "20");
    munit_assert_string_equal(msg.body.data[1].value, "raw\001data\00110=is\001weird");
    munit_assert_string_equal(msg.trailer.data[0].value, "046");

    cfix_message_fini(&msg);
    cfix_ring_fini(&ring);
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
    cfix_ring_t    ring;
    cfix_message_t msg;
    unit_parser_parse(text, result, &ring, &msg);
    cfix_message_fini(&msg);
    cfix_ring_fini(&ring);
}

void unit_parser_parse(const char *text, int result, cfix_ring_t *ring, cfix_message_t *out)
{
    munit_logf(MUNIT_LOG_INFO, "Text: %s", text);

    munit_assert_int(cfix_ring_init(ring, getpagesize()), ==, 0);
    write_string_to_ring(ring, text);

    cfix_tag_entry_t entries[150] = {0};
    cfix_tag_table_t table = {.data = entries, .size = sizeof(entries) / sizeof(entries[0])};
    entries[35].is_header_field = 1;
    entries[34].is_header_field = 1;
    entries[49].is_header_field = 1;
    entries[52].is_header_field = 1;
    entries[56].is_header_field = 1;
    entries[96].is_data_type = 1;

    cfix_parser_config_t config = {
        .tag_table = &table,
        .begin_string = "FIX.4.0",
        .validate_begin_string = 1,
        .validate_checksum = 1,
        .separator_field = '\001',
        .separator_tag_value = '=',
    };
    cfix_parser_t parser;
    cfix_parser_init(&parser, &config);

    // Execute.
    cfix_message_init(out);
    munit_assert_int(cfix_parser_get(&parser, ring, out), ==, result);
}
