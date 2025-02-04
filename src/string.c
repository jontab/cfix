#include "cfix/string.h"
#include "cfix/memory.h"
#include <string.h>

int cfix_string_init(cfix_string_t *self, const char *text)
{
    self->length = strlen(text);
    if (self->length < sizeof(self->small))
    {
        strncpy(self->small, text, sizeof(self->small));
        return 0;
    }
    else
    {
        self->large = strdup(text);
        return self->large ? 0 : -1;
    }
}

void cfix_string_fini(cfix_string_t *self)
{
    if (self->length >= sizeof(self->small))
    {
        DELETE(self->large, free);
    }

    CLEAR(self);
}

char *cfix_string_data(cfix_string_t *self)
{
    return (self->length < sizeof(self->small)) ? self->small : self->large;
}
