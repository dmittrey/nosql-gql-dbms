#pragma once

#include "utils.h"
#include "table.h"

typedef struct
{
    sectoff_t free_space;
    fileoff_t next_ptr;
    sectoff_t last_item_ptr;
    sectoff_t first_record_ptr;
} section_header_entity;