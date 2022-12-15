#pragma once

#include <inttypes.h>

#include "utils.h"

typedef void (*write_func_t)(void *);
typedef void (*read_func_t)(void *);

typedef struct
{
    fileoff_t free_space;
    fileoff_t next;
    fileoff_t last_item_ptr;    // Pointer to first free cell(after items)
    fileoff_t first_record_ptr; // Pointer to last free cell(before records)
    write_func_t write_func;
    read_func_t read_func;
} page_t;