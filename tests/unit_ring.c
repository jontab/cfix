#include "cfix/ring.h"
#include <munit.h>
#include <unistd.h> // getpagesize

MunitResult unit_ring_init(const MunitParameter params[], void *data);
MunitResult unit_ring_reserve(const MunitParameter params[], void *data);
MunitResult unit_ring_commit(const MunitParameter params[], void *data);
MunitResult unit_ring_read(const MunitParameter params[], void *data);
MunitResult unit_ring_consume(const MunitParameter params[], void *data);
MunitResult unit_ring_consume_wrap(const MunitParameter params[], void *data);

MunitTest unit_ring_tests[] = {
    {"/init", unit_ring_init, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/reserve", unit_ring_reserve, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/commit", unit_ring_commit, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/read", unit_ring_read, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/consume", unit_ring_consume, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {"/consume_wrap", unit_ring_consume_wrap, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
    {NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL},
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

MunitResult unit_ring_init(const MunitParameter params[], void *data)
{
    (void)params;
    (void)data;
    cfix_ring_t ring;
    munit_assert_int(cfix_ring_init(&ring, getpagesize()), ==, 0);
    munit_assert_int(ring.file, >, 0);

    // Execute.
    ring.data[0] = 'a';
    munit_assert_char(ring.data[0], ==, 'a');
    munit_assert_char(ring.data[ring.capacity], ==, 'a');

    ring.data[ring.capacity - 1] = 'b';
    munit_assert_char(ring.data[ring.capacity - 1], ==, 'b');
    munit_assert_char(ring.data[ring.capacity * 2 - 1], ==, 'b');

    // Cleanup.
    cfix_ring_fini(&ring);
    return MUNIT_OK;
}

MunitResult unit_ring_reserve(const MunitParameter params[], void *data)
{
    (void)params;
    (void)data;
    cfix_ring_t ring;
    munit_assert_int(cfix_ring_init(&ring, getpagesize()), ==, 0);

    // Execute.
    size_t available;
    munit_assert_ptr_equal(cfix_ring_reserve(&ring, &available), &ring.data[0]);
    munit_assert_size(available, ==, ring.capacity);

    // Cleanup.
    cfix_ring_fini(&ring);
    return MUNIT_OK;
}

MunitResult unit_ring_commit(const MunitParameter params[], void *data)
{
    (void)params;
    (void)data;
    cfix_ring_t ring;
    munit_assert_int(cfix_ring_init(&ring, getpagesize()), ==, 0);

    // Execute.
    size_t available;
    char  *write = cfix_ring_reserve(&ring, &available);
    memset(write, 'a', 10);      // Write.
    cfix_ring_commit(&ring, 10); // Commit.
    munit_assert_ptr_equal(cfix_ring_reserve(&ring, &available), &ring.data[10]);
    munit_assert_size(available, ==, ring.capacity - 10);

    // Cleanup.
    cfix_ring_fini(&ring);
    return MUNIT_OK;
}

MunitResult unit_ring_read(const MunitParameter params[], void *data)
{
    (void)params;
    (void)data;
    cfix_ring_t ring;
    munit_assert_int(cfix_ring_init(&ring, getpagesize()), ==, 0);

    // Execute.
    size_t available;
    munit_assert_ptr_equal(cfix_ring_read(&ring, &available), &ring.data[0]);
    munit_assert_size(available, ==, 0);

    // Cleanup.
    cfix_ring_fini(&ring);
    return MUNIT_OK;
}

MunitResult unit_ring_consume(const MunitParameter params[], void *data)
{
    (void)params;
    (void)data;
    cfix_ring_t ring;
    munit_assert_int(cfix_ring_init(&ring, getpagesize()), ==, 0);

    // Execute.
    size_t available;
    char  *write = cfix_ring_reserve(&ring, &available);
    memset(write, 'a', 10);      // Write.
    cfix_ring_commit(&ring, 10); // Commit.

    char *read = cfix_ring_read(&ring, &available);
    munit_assert_size(available, ==, 10);
    for (size_t i = 0; i < 10; i++)
        munit_assert_char(read[i], ==, 'a'); // Read.
    cfix_ring_consume(&ring, 10);            // Consume.

    munit_assert_ptr_equal(cfix_ring_read(&ring, &available), &ring.data[10]);
    munit_assert_size(available, ==, 0);

    // Cleanup.
    cfix_ring_fini(&ring);
    return MUNIT_OK;
}

/**
 * @note
 * In order for the `head` and `tail` indices to not get out of hand and get too large (which might cause problems with
 * long-running usage of the ring buffer), we periodically shift both the `head` and `tail` indices if they both exceed
 * the capacity of the buffer. Since the values at those indices are evaluated modulo the capacity of the buffer, there
 * is no logical difference in subtracting that capacity.
 */
MunitResult unit_ring_consume_wrap(const MunitParameter params[], void *data)
{
    (void)params;
    (void)data;
    cfix_ring_t ring;
    munit_assert_int(cfix_ring_init(&ring, getpagesize()), ==, 0);
    cfix_ring_commit(&ring, 10);  // Offset (for robustness).
    cfix_ring_consume(&ring, 10); // Offset (for robustness).

    // Execute.
    cfix_ring_commit(&ring, ring.capacity);
    cfix_ring_consume(&ring, ring.capacity);
    size_t available;
    munit_assert_ptr_equal(cfix_ring_reserve(&ring, &available), &ring.data[10]);
    munit_assert_size(available, ==, ring.capacity);
    munit_assert_ptr_equal(cfix_ring_read(&ring, &available), &ring.data[10]);
    munit_assert_size(available, ==, 0);

    // Cleanup.
    cfix_ring_fini(&ring);
    return MUNIT_OK;
}
