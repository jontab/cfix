#include "cfix/message.h"
#include <munit.h>
#include <stdio.h> // snprintf

MunitResult unit_field_list_append(const MunitParameter params[], void *data);

MunitTest unit_message_tests[] = {
    {"/field_list_append", unit_field_list_append, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

MunitResult unit_field_list_append(const MunitParameter params[], void *data)
{
    (void)params;
    (void)data;
    cfix_message_field_list_t list;
    cfix_message_field_list_init(&list);

    // Execute.
    munit_assert_int(cfix_message_field_list_append(&list, 0, "0"), ==, 0);
    munit_assert_int(cfix_message_field_list_append(&list, 1, "1"), ==, 0);
    munit_assert_int(cfix_message_field_list_append(&list, 2, "2"), ==, 0);
    munit_assert_int(cfix_message_field_list_append(&list, 3, "3"), ==, 0);
    munit_assert_int(cfix_message_field_list_append(&list, 4, "4"), ==, 0);
    munit_assert_int(cfix_message_field_list_append(&list, 5, "5"), ==, 0);
    munit_assert_int(cfix_message_field_list_append(&list, 6, "6"), ==, 0);
    munit_assert_int(cfix_message_field_list_append(&list, 7, "7"), ==, 0);
    munit_assert_size(list.size, ==, 8);
    munit_assert_size(list.capacity, >=, 8);
    for (int i = 0; i < 8; i++)
    {
        char number[2];
        snprintf(number, sizeof(number), "%d", i);
        munit_assert_int(list.data[i].tag, ==, i);
        munit_assert_string_equal(list.data[i].value, number);
    }

    // Cleanup.
    cfix_message_field_list_fini(&list);
    return MUNIT_OK;
}
