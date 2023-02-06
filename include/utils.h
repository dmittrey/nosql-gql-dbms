#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define my_malloc(T) ((T *)malloc(sizeof(T)))
#define my_malloc_array(T, count) ((T *)malloc(sizeof(T) * count))

#define FWRITE_OR_FAIL(DATA_PTR, DATA_SIZE, FILP)  \
    if (fwrite(DATA_PTR, DATA_SIZE, 1, FILP) != 1) \
    return FAILED

#define FREAD_OR_FAIL(DATA_PTR, DATA_SIZE, FILP)  \
    if (fread(DATA_PTR, DATA_SIZE, 1, FILP) != 1) \
    return FAILED

#define FSEEK_OR_FAIL(FILE, OFFSET)         \
    if (fseek(FILE, OFFSET, SEEK_SET) != 0) \
    return FAILED

// Own type of string to simplify handling
typedef struct
{
    const char *val;
    int count;
} string_t;

typedef enum
{
    OK = 0,
    FAILED = 1
} PerformStatus;