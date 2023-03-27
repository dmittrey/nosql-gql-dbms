#pragma once

#include "utils/utils.h"
#include "utils/table.h"

typedef struct File_head
{
    Fileoff lst_sect_ptr; // Points first free bit after sects
    FILE *filp;
} File_head;

