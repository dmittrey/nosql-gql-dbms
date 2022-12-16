#pragma once

#include "utils.h"

#include <stdio.h>

typedef struct
{
    sectoff_t free_space;
    sectoff_t next;             // Nullable
    sectoff_t last_item_ptr;    // Pointer to first free cell(after items)
    sectoff_t first_record_ptr; // Pointer to last free cell(before records)
    fileoff_t fileoff;
    FILE *filp;
} section_page_t;