#pragma once

#include "utils.h"
#include "table.h"

typedef struct
{
    sectoff_t free_space;
    fileoff_t next_ptr;
    sectoff_t lst_itm_ptr;
    sectoff_t fst_rec_ptr;
} sect_head_entity_t;

sect_head_entity_t *sect_head_entity_new();

void sect_head_entity_dtor(sect_head_entity_t *);