#ifndef TEST_CONTEXT_H
#define TEST_CONTEXT_H

#include <pthread.h>
#include <stdbool.h>

/******************************************************************************/
/* Typedefs                                                                   */
/******************************************************************************/

typedef struct cfix_test_context_s cfix_test_context_t;

/******************************************************************************/
/* Structs                                                                    */
/******************************************************************************/

struct cfix_test_context_s
{
    bool            is_done;
    pthread_mutex_t lock;
    pthread_cond_t  wake;
};

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void cfix_test_context_init(cfix_test_context_t *self);
void cfix_test_context_wait(cfix_test_context_t *self);
void cfix_test_context_done(cfix_test_context_t *self);

#endif // TEST_CONTEXT_H
