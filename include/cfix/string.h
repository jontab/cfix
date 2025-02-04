#ifndef CFIX_STRING_H
#define CFIX_STRING_H

#include <stddef.h>

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct cfix_string_s cfix_string_t;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct cfix_string_s
{
    char  small[16];
    char *large;
    int   length;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

int   cfix_string_init(cfix_string_t *self, const char *text);
void  cfix_string_fini(cfix_string_t *self);
char *cfix_string_data(cfix_string_t *self);

#endif // CFIX_STRING_H
