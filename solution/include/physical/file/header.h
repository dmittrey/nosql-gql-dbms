#pragma once

#include "table.h"

typedef struct 
{
    fileoff_t lst_sect_ptr; // Points first free bit after sects
} file_head_t;