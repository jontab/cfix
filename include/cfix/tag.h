#ifndef CFIX_TAG_H
#define CFIX_TAG_H

#include <stdbool.h>

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct TagContext TagContext;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct TagContext
{
    bool (*is_header_tag)(TagContext *self, int tag);  // Location.
    bool (*is_trailer_tag)(TagContext *self, int tag); // Location.
    bool (*is_data_tag)(TagContext *self, int tag);
};

#endif // CFIX_TAG_H
