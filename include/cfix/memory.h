#ifndef CFIX_MEMORY_H
#define CFIX_MEMORY_H

#include <stdlib.h>

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define NEW(X)                                                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        X = calloc(1, sizeof(*(X)));                                                                                   \
    } while (0)

#define DELETE(X, FREE)                                                                                                \
    do                                                                                                                 \
    {                                                                                                                  \
        FREE(X);                                                                                                       \
        X = NULL;                                                                                                      \
    } while (0)

#define CLEAR(X)                                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        memset(X, 0, sizeof(*(X)));                                                                                    \
    } while (0)

#endif // CFIX_MEMORY_H
