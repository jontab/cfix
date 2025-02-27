#ifndef CFIX_RING_H
#define CFIX_RING_H

#include <stddef.h>

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct cfix_ring_s cfix_ring_t;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct cfix_ring_s
{
    char  *data;
    size_t capacity;
    size_t head;
    size_t tail;
    int    file;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

int   cfix_ring_init(cfix_ring_t *self, size_t capacity);
void  cfix_ring_fini(cfix_ring_t *self);
char *cfix_ring_reserve(cfix_ring_t *self, size_t *available);
char *cfix_ring_read(cfix_ring_t *self, size_t *available);
void  cfix_ring_commit(cfix_ring_t *self, size_t by);
void  cfix_ring_consume(cfix_ring_t *self, size_t by);

#endif // CFIX_RING_H
