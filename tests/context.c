#include "context.h"
#include "munit.h"

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

void cfix_test_context_init(cfix_test_context_t *self)
{
    self->is_done = false;
    munit_assert_int(pthread_mutex_init(&self->lock, NULL), >=, 0);
    munit_assert_int(pthread_cond_init(&self->wake, NULL), >=, 0);
}

void cfix_test_context_wait(cfix_test_context_t *self)
{
    munit_assert_int(pthread_mutex_lock(&self->lock), >=, 0);
    while (!self->is_done)
        munit_assert_int(pthread_cond_wait(&self->wake, &self->lock), >=, 0);
    munit_assert_int(pthread_mutex_unlock(&self->lock), >=, 0);
}

void cfix_test_context_done(cfix_test_context_t *self)
{
    munit_assert_int(pthread_mutex_lock(&self->lock), >=, 0);
    self->is_done = true;
    munit_assert_int(pthread_cond_signal(&self->wake), >=, 0);
    munit_assert_int(pthread_mutex_unlock(&self->lock), >=, 0);
}
