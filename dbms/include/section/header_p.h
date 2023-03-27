#pragma once

#include "utils/utils.h"
#include "utils/table.h"

typedef struct Sect_head
{
    size_t free_space;
    Fileoff next_ptr;    // Nullable
    Sectoff lst_itm_ptr; // Pointer to first free cell(after items)
    Sectoff fst_rec_ptr; // Pointer to last free cell(before records)
    Fileoff file_offset; // Section offset from file start
    FILE *filp;
} Sect_head;

int sect_head_cmp(Sect_head * this, Sect_head * other);