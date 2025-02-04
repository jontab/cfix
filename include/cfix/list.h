#ifndef CFIX_LIST_H
#define CFIX_LIST_H

#include "cfix/memory.h"

#define LIST_GENERATE_HEADER(PREFIX, TYPE)                                                                             \
    typedef struct PREFIX##_s PREFIX##_t;                                                                              \
    struct PREFIX##_s                                                                                                  \
    {                                                                                                                  \
        TYPE *data;                                                                                                    \
        int   size;                                                                                                    \
        int   capacity;                                                                                                \
    };                                                                                                                 \
    void PREFIX##_fini(PREFIX##_t *self);                                                                              \
    int  PREFIX##_reserve(PREFIX##_t *self, int size);                                                                 \
    int  PREFIX##_reserve_exact(PREFIX##_t *self, int size);                                                           \
    int  PREFIX##_insert(PREFIX##_t *self, TYPE data, int at);                                                         \
    int  PREFIX##_append(PREFIX##_t *self, TYPE data);

#define LIST_GENERATE_SOURCE(PREFIX, TYPE)                                                                             \
    void PREFIX##_fini(PREFIX##_t *self)                                                                               \
    {                                                                                                                  \
        if (self->data)                                                                                                \
        {                                                                                                              \
            DELETE(self->data, free);                                                                                  \
        }                                                                                                              \
        self->size = self->capacity = 0;                                                                               \
    }                                                                                                                  \
    int PREFIX##_reserve(PREFIX##_t *self, int size)                                                                   \
    {                                                                                                                  \
        if (self->capacity < size)                                                                                     \
        {                                                                                                              \
            int   new_capacity = size + 11;                                                                            \
            TYPE *new_data = realloc(self->data, new_capacity * sizeof(TYPE));                                         \
            if (new_data)                                                                                              \
            {                                                                                                          \
                self->capacity = new_capacity;                                                                         \
                self->data = new_data;                                                                                 \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                return -1;                                                                                             \
            }                                                                                                          \
        }                                                                                                              \
        return 0;                                                                                                      \
    }                                                                                                                  \
    int PREFIX##_reserve_exact(PREFIX##_t *self, int size)                                                             \
    {                                                                                                                  \
        if (self->capacity < size)                                                                                     \
        {                                                                                                              \
            int   new_capacity = size;                                                                                 \
            TYPE *new_data = realloc(self->data, new_capacity * sizeof(TYPE));                                         \
            if (new_data)                                                                                              \
            {                                                                                                          \
                self->capacity = new_capacity;                                                                         \
                self->data = new_data;                                                                                 \
            }                                                                                                          \
            else                                                                                                       \
            {                                                                                                          \
                return -1;                                                                                             \
            }                                                                                                          \
        }                                                                                                              \
        return 0;                                                                                                      \
    }                                                                                                                  \
    int PREFIX##_insert(PREFIX##_t *self, TYPE data, int at)                                                           \
    {                                                                                                                  \
        if (cfix_message_fields_reserve(self, self->size + 1) < 0)                                                     \
        {                                                                                                              \
            return -1;                                                                                                 \
        }                                                                                                              \
        memmove(&self->data[at + 1], &self->data[at], (self->size - at) * sizeof(TYPE));                               \
        self->data[at] = (data);                                                                                       \
        self->size++;                                                                                                  \
        return 0;                                                                                                      \
    }                                                                                                                  \
    int PREFIX##_append(PREFIX##_t *self, TYPE data)                                                                   \
    {                                                                                                                  \
        return PREFIX##_insert(self, data, self->size);                                                                \
    }

#endif // CFIX_LIST_H
