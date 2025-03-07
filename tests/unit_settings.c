#include <cfix/settings.h>
#include <munit.h>

MunitResult unit_settings_init_from_file(const MunitParameter params[], void *data);

MunitTest unit_settings_tests[] = {
    {"/init_from_file", unit_settings_init_from_file, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

MunitResult unit_settings_init_from_file(const MunitParameter params[], void *data)
{
    cfix_settings_t settings;
    cfix_settings_init_from_file(&settings, "data/settings_example.yaml");
    munit_assert_int(settings.sessions_size, ==, 2);
    munit_assert_string_equal(settings.sessions[0].id, "MySession");
    munit_assert_string_equal(settings.sessions[0].sender_comp_id, "SenderId");
    munit_assert_string_equal(settings.sessions[0].target_comp_id, "TargetId");
    munit_assert_string_equal(settings.sessions[1].id, "MySession2");
    munit_assert_string_equal(settings.sessions[1].sender_comp_id, "SenderId2");
    munit_assert_string_equal(settings.sessions[1].target_comp_id, "TargetId2");
    cfix_settings_fini(&settings);
    return 0;
}
