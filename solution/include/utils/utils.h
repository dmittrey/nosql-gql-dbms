#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define my_clear(PTR) memset(PTR, 0, sizeof(typeof(*PTR)))
#define my_malloc(T) ((T *)malloc(sizeof(T)))
#define my_calloc(T) ((T *)calloc(1, sizeof(T)))

#define my_malloc_array(T, count) ((T *)malloc(sizeof(T) * count))

#define FWRITE_OR_FAIL(DATA_PTR, DATA_SIZE, FILP)          \
    if (fwrite((void *)DATA_PTR, DATA_SIZE, 1, FILP) != 1) \
    return FAILED

#define FREAD_OR_FAIL(DATA_PTR, DATA_SIZE, FILP)          \
    if (fread((void *)DATA_PTR, DATA_SIZE, 1, FILP) != 1) \
    return FAILED

#define FSEEK_OR_FAIL(FILP, OFFSET)         \
    if (fseek(FILP, OFFSET, SEEK_SET) != 0) \
    return FAILED

#define RA_FWRITE_OR_FAIL(DATA_PTR, DATA_SIZE, OFFSET, FILP) \
    FSEEK_OR_FAIL(FILP, OFFSET);                             \
    FWRITE_OR_FAIL(DATA_PTR, DATA_SIZE, FILP)

#define RA_FREAD_OR_FAIL(DATA_PTR, DATA_SIZE, OFFSET, FILP) \
    FSEEK_OR_FAIL(FILP, OFFSET);                            \
    FREAD_OR_FAIL(DATA_PTR, DATA_SIZE, FILP)

#define DO_OR_FAIL(FUNC) \
    if (FUNC == FAILED)  \
    return FAILED

#define SAVE_FILP(FILP, FUNC)    \
    long prev_ptr = ftell(FILP); \
    FUNC;                        \
    FSEEK_OR_FAIL(FILP, prev_ptr);

typedef enum
{
    OK = 0,
    FAILED = 1
} Status;