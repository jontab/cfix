#ifndef CFIX_SETTINGS_H
#define CFIX_SETTINGS_H

#include <stdio.h>

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct cfix_settings_session_s cfix_settings_session_t;
typedef struct cfix_settings_s         cfix_settings_t;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct cfix_settings_session_s
{
    char id[BUFSIZ];
    char sender_comp_id[BUFSIZ];
    char target_comp_id[BUFSIZ];
};

struct cfix_settings_s
{
    cfix_settings_session_t *sessions;
    int                      sessions_size;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

int  cfix_settings_init_from_file(cfix_settings_t *self, const char *settings_path);
void cfix_settings_fini(cfix_settings_t *self);

#endif // CFIX_SETTINGS_H
