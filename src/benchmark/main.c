#include <assert.h>
#include <cfix/parser.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define ITERATIONS 10000000
#define MESSAGE    "8=FIX.4.0\0019=30\00195=20\00196=raw\001data\00110=is\001weird\00110=046\001"

long time_us(void);
void write_to_ring(cfix_ring_t *ring, const char *text);

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    cfix_ring_t      ring;
    cfix_parser_t    parser;
    cfix_tag_entry_t tag_entries[150] = {0};
    tag_entries[35].is_header_field = 1;
    tag_entries[34].is_header_field = 1;
    tag_entries[49].is_header_field = 1;
    tag_entries[52].is_header_field = 1;
    tag_entries[56].is_header_field = 1;
    tag_entries[96].is_data_type = 1;
    cfix_tag_table_t tag_table = {
        .data = tag_entries,
        .size = sizeof(tag_entries) / sizeof(tag_entries[0]),
    };
    cfix_parser_config_t config = {
        .begin_string = "FIX.4.0",
        .validate_begin_string = 1,
        .validate_checksum = 0,
        .separator_field = '\001',
        .separator_tag_value = '=',
        .tag_table = &tag_table,
    };
    cfix_ring_init(&ring, getpagesize());
    cfix_parser_init(&parser, &config);

    // Prepare.
    cfix_message_t dummy;
    cfix_message_init(&dummy);
    long total_time_begin = time_us();
    long min_time = -1;
    long max_time = -1;

    // Execute.
    for (long i = 0; i < ITERATIONS; i++)
    {
        // Prepare.
        write_to_ring(&ring, MESSAGE);
        long time_begin = time_us();

        // Execute.
        int result = cfix_parser_get(&parser, &ring, &dummy);
        assert(result == 0);

        // Finish.
        long time_end = time_us();
        long time = time_end - time_begin;
        if ((min_time < 0) || (time < min_time))
        {
            min_time = time;
        }

        if ((max_time < 0) || (time > max_time))
        {
            max_time = time;
        }

        cfix_message_field_list_clear(&dummy.header);
        cfix_message_field_list_clear(&dummy.body);
        cfix_message_field_list_clear(&dummy.trailer);
    }

    // Finish.
    long total_time_end = time_us();
    long total_time = total_time_end - total_time_begin;
    fprintf(stdout, "Total time for %d iterations: %ld usec\n", ITERATIONS, total_time);
    fprintf(stdout, "Average time per iteration: %lf usec\n", (double)total_time / ITERATIONS);
    fprintf(stdout, "Minimum time per iteration: %ld usec\n", min_time);
    fprintf(stdout, "Maximum time per iteration: %ld usec\n", max_time);
    cfix_ring_fini(&ring);
    return 0;
}

long time_us(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000000 + tv.tv_usec;
}

void write_to_ring(cfix_ring_t *ring, const char *text)
{
    size_t available;
    char  *write = cfix_ring_reserve(ring, &available);
    assert(strlen(text) <= available);
    strncpy(write, text, strlen(text));
    cfix_ring_commit(ring, strlen(text));
}
