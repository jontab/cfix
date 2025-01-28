#ifndef CFIX_TCP_CLOSE_H
#define CFIX_TCP_CLOSE_H

#include <unistd.h>

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

#define CLOSE(X)                                                                                                       \
    do                                                                                                                 \
    {                                                                                                                  \
        close(X);                                                                                                      \
        X = -1;                                                                                                        \
    } while (0)

#endif // CFIX_TCP_CLOSE_H
