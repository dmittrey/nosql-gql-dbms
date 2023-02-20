#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define my_malloc(T) ((T *)malloc(sizeof(T)))
#define my_malloc_array(T, count) ((T *)malloc(sizeof(T) * count))

#define FWRITE_OR_FAIL(DATA_PTR, DATA_SIZE, FILP)          \
    if (fwrite((void *)DATA_PTR, DATA_SIZE, 1, FILP) != 1) \
    return FAILED

#define FREAD_OR_FAIL(DATA_PTR, DATA_SIZE, FILP)          \
    if (fread((void *)DATA_PTR, DATA_SIZE, 1, FILP) != 1) \
    return FAILED

#define FSEEK_OR_FAIL(FILE, OFFSET)         \
    if (fseek(FILE, OFFSET, SEEK_SET) != 0) \
    return FAILED

#define RANDOM_ACCESS_FWRITE_OR_FAIL(DATA_PTR, DATA_SIZE, OFFSET, FILP) \
    if (fseek(FILP, OFFSET, SEEK_SET) != 0)                             \
        return FAILED;                                                  \
    if (fwrite((void *)DATA_PTR, DATA_SIZE, 1, FILP) != 1)              \
    return FAILED

#define RANDOM_ACCESS_FREAD_OR_FAIL(DATA_PTR, DATA_SIZE, OFFSET, FILP) \
    if (fseek(FILP, OFFSET, SEEK_SET) != 0)                            \
        return FAILED;                                                 \
    if (fread((void *)DATA_PTR, DATA_SIZE, 1, FILP) != 1)              \
    return FAILED

// Own type of string to simplify handling
typedef struct
{
    char *val;
    size_t count;
} string_t;

string_t* string_new();
void string_ctor(string_t * const, const char * const, const size_t);
void string_dtor(string_t *);

size_t string_get_size(const string_t str);

typedef enum
{
    OK = 0,
    FAILED = 1
} PerformStatus;