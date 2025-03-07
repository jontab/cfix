#include <cfix/settings.h>
#include <yaml.h>

/******************************************************************************/
/* Enums                                                                      */
/******************************************************************************/

enum cfix_settings_state_e
{
    STATE_INITIAL,
    STATE_EXPECT_SESSIONS,
    STATE_EXPECT_SESSIONS_SEQUENCE,
    STATE_EXPECT_SESSIONS_SEQUENCE_MAPPING,
    STATE_EXPECT_KEY,
    STATE_EXPECT_VALUE,
    STATE_DONE,
};

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef enum cfix_settings_state_e cfix_settings_state_t;

int  on_mapping_start(cfix_settings_state_t *st, yaml_event_t *ev);
int  on_mapping_end(cfix_settings_t *self, cfix_settings_state_t *st, cfix_settings_session_t *sess);
int  on_scalar(cfix_settings_state_t *st, yaml_event_t *ev, char *key, size_t key_size, cfix_settings_session_t *sess);
int  on_sequence_start(cfix_settings_state_t *st, yaml_event_t *ev);
void on_error(const char *text, yaml_event_t *ev);

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

int cfix_settings_init_from_file(cfix_settings_t *self, const char *settings_path)
{
    self->sessions = NULL;
    self->sessions_size = 0;

    //
    FILE *file = fopen(settings_path, "r");
    if (!file)
    {
        perror("fopen");
        return -1;
    }

    yaml_parser_t parser;
    yaml_event_t  event;
    if (!yaml_parser_initialize(&parser))
    {
        fprintf(stderr, "cfix_settings_init_from_file: failed to initialize parser\n");
        fclose(file);
        return -1;
    }

    yaml_parser_set_input_file(&parser, file);
    cfix_settings_session_t session = {0};
    cfix_settings_state_t   state = STATE_INITIAL;
    char                    key[BUFSIZ] = {0};
    while (!(state == STATE_DONE))
    {
        if (!yaml_parser_parse(&parser, &event))
        {
            fprintf(stderr, "cfix_settings_init_from_file: parse error: %d\n", parser.error);
            goto error;
        }

        switch (event.type)
        {
        case YAML_MAPPING_START_EVENT:
            if (on_mapping_start(&state, &event) < 0)
                goto error;
            break;
        case YAML_MAPPING_END_EVENT:
            if (on_mapping_end(self, &state, &session) < 0)
                goto error;
            break;
        case YAML_SCALAR_EVENT:
            if (on_scalar(&state, &event, key, sizeof(key), &session) < 0)
                goto error;
            break;
        case YAML_SEQUENCE_START_EVENT:
            if (on_sequence_start(&state, &event) < 0)
                goto error;
            break;
        case YAML_STREAM_END_EVENT:
            state = STATE_DONE;
            break;
        default:
            break;
        }

        yaml_event_delete(&event);
    }

    yaml_parser_delete(&parser);
    fclose(file);
    return 0;

error:
    yaml_event_delete(&event);   // Cleanup.
    yaml_parser_delete(&parser); // Cleanup.
    fclose(file);                // Cleanup.
    return -1;
}

void cfix_settings_fini(cfix_settings_t *self)
{
    free(self->sessions);
}

int on_mapping_start(cfix_settings_state_t *st, yaml_event_t *ev)
{
    switch (*st)
    {
    case STATE_INITIAL:
        *st = STATE_EXPECT_SESSIONS;
        break;
    case STATE_EXPECT_SESSIONS_SEQUENCE_MAPPING:
        *st = STATE_EXPECT_KEY;
        break;
    default:
        on_error("unexpected mapping", ev);
        return -1;
    }

    return 0;
}

int on_mapping_end(cfix_settings_t *self, cfix_settings_state_t *st, cfix_settings_session_t *sess)
{
    if (*st == STATE_EXPECT_KEY)
    {
        int                      new_arr_size = self->sessions_size + 1;
        cfix_settings_session_t *new_arr = realloc(self->sessions, new_arr_size * sizeof(self->sessions[0]));
        if (!new_arr)
        {
            perror("realloc");
            return -1;
        }

        new_arr[new_arr_size - 1] = *sess;
        self->sessions = new_arr;
        self->sessions_size = new_arr_size;
        *st = STATE_EXPECT_SESSIONS_SEQUENCE_MAPPING;
    }

    return 0;
}

int on_scalar(cfix_settings_state_t *st, yaml_event_t *ev, char *key, size_t key_size, cfix_settings_session_t *sess)
{
    switch (*st)
    {
    case STATE_EXPECT_SESSIONS:
        if (strcmp((const char *)ev->data.scalar.value, "sessions") == 0)
        {
            *st = STATE_EXPECT_SESSIONS_SEQUENCE;
        }
        else
        {
            on_error("expected \"sessions\"", ev);
            return -1;
        }

        break;
    case STATE_EXPECT_KEY:
        strncpy(key, (const char *)ev->data.scalar.value, key_size);
        *st = STATE_EXPECT_VALUE;
        break;
    case STATE_EXPECT_VALUE:
        if (strcmp(key, "id") == 0)
        {
            strncpy(sess->id, (const char *)ev->data.scalar.value, sizeof(sess->id));
        }
        else if (strcmp(key, "sender_comp_id") == 0)
        {
            strncpy(sess->sender_comp_id, (const char *)ev->data.scalar.value, sizeof(sess->sender_comp_id));
        }
        else if (strcmp(key, "target_comp_id") == 0)
        {
            strncpy(sess->target_comp_id, (const char *)ev->data.scalar.value, sizeof(sess->target_comp_id));
        }

        *st = STATE_EXPECT_KEY;
        break;
    default:
        on_error("unexpected scalar", ev);
        return -1;
    }

    return 0;
}

int on_sequence_start(cfix_settings_state_t *st, yaml_event_t *ev)
{
    switch (*st)
    {
    case STATE_EXPECT_SESSIONS_SEQUENCE:
        *st = STATE_EXPECT_SESSIONS_SEQUENCE_MAPPING;
        break;
    default:
        on_error("unexpected sequence", ev);
        return -1;
    }

    return 0;
}

void on_error(const char *text, yaml_event_t *ev)
{
    int y = (int)ev->start_mark.line;
    int x = (int)ev->start_mark.column;
    fprintf(stderr, "cfix_settings_init_from_file: %s at %d:%d\n", text, y, x);
}
