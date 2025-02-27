#include "cfix/ring.h"
#include <fcntl.h>    // O_CREAT
#include <stdio.h>    // fopen
#include <sys/mman.h> // mmap
#include <unistd.h>   // close

unsigned mmap_rand(FILE *random);
int      mmap_create_memfd(size_t capacity);
char    *mmap_create_magic_buffer(size_t capacity, int fd);

/******************************************************************************/
/* Methods                                                                    */
/******************************************************************************/

int cfix_ring_init(cfix_ring_t *self, size_t capacity)
{
    int   file = mmap_create_memfd(capacity);
    char *data = mmap_create_magic_buffer(capacity, file);
    if (data)
    {
        self->data = data;
        self->capacity = capacity;
        self->head = self->tail = 0;
        self->file = file;
        return 0;
    }
    else
    {
        return -1;
    }
}

void cfix_ring_fini(cfix_ring_t *self)
{
    munmap(self->data, self->capacity * 2);
    close(self->file);
}

char *cfix_ring_reserve(cfix_ring_t *self, size_t *available)
{
    // We basically want `self->head - self->tail (mod self->capacity)`.
    *available = self->head + self->capacity - self->tail;
    return &self->data[self->tail];
}

char *cfix_ring_read(cfix_ring_t *self, size_t *available)
{
    *available = self->tail - self->head;
    return &self->data[self->head];
}

void cfix_ring_commit(cfix_ring_t *self, size_t by)
{
    self->tail += by;
}

void cfix_ring_consume(cfix_ring_t *self, size_t by)
{
    self->head += by;
    if (self->head >= self->capacity)
    {
        self->head -= self->capacity;
        self->tail -= self->capacity;
    }
}

/******************************************************************************/
/* Private                                                                    */
/******************************************************************************/

unsigned mmap_rand(FILE *random)
{
    unsigned value;
    fread(&value, sizeof(value), 1, random);
    return value;
}

int mmap_create_memfd(size_t capacity)
{
    FILE *random = fopen("/dev/urandom", "rb");
    if (!random)
    {
        perror("fopen");
        return -1;
    }

    char alphabet[] = "0123456789abcdef";
    char path[] = {
        '/',
        alphabet[mmap_rand(random) % 16], // 1.
        alphabet[mmap_rand(random) % 16],
        alphabet[mmap_rand(random) % 16],
        alphabet[mmap_rand(random) % 16],
        alphabet[mmap_rand(random) % 16],
        alphabet[mmap_rand(random) % 16],
        alphabet[mmap_rand(random) % 16],
        alphabet[mmap_rand(random) % 16], // 8.
        '\0',
    };
    fclose(random);

    int fd = shm_open(path, O_CREAT | O_RDWR, 0666);
    if (fd < 0)
    {
        perror("shm_open");
        return -1;
    }

    if (ftruncate(fd, capacity) < 0)
    {
        perror("ftruncate");
        close(fd);
        return -1;
    }

    return fd;
}

char *mmap_create_magic_buffer(size_t capacity, int fd)
{
    char *one = mmap(NULL, capacity * 2, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (one == MAP_FAILED)
    {
        perror("mmap");
        return NULL;
    }

    if ((mmap(one + (0 * capacity), capacity, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0) == MAP_FAILED) ||
        (mmap(one + (1 * capacity), capacity, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_FIXED, fd, 0) == MAP_FAILED))
    {
        perror("mmap");
        return NULL;
    }

    return one;
}
