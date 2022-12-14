#pragma once

#include <inttypes.h>

#include "utils.h"

typedef uint64_t fileoff_t;

typedef void (*insert_func_t)(void *);
typedef void (*update_func_t)(void *);
typedef void (*remove_func_t)(void *);
typedef void (*read_func_t)(void *);

typedef struct
{
    fileoff_t free_space;
    fileoff_t next;
    fileoff_t last_item_ptr;    // Pointer to first free cell(after items)
    fileoff_t first_record_ptr; // Pointer to last free cell(before records)
    insert_func_t insert_func;
    update_func_t update_func;
    remove_func_t remove_func;
    read_func_t read_func;
} page_t;