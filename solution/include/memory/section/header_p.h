#pragma once

#include "utils.h"
#include "table.h"

typedef struct sect_head_t
{
    sectoff_t free_space;
    fileoff_t next_ptr;    // Nullable
    sectoff_t lst_itm_ptr; // Pointer to first free cell(after items)
    sectoff_t fst_rec_ptr; // Pointer to last free cell(before records)
    fileoff_t sect_off;    // Section offset from file start
    FILE *filp;
} sect_head_t;