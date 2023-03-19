#pragma once

#include "utils/utils.h"
#include "utils/table.h"

typedef struct
{
    size_t name_size;
    Sectoff name_ptr;
    size_t attr_cnt;
} Type_entity;

Type_entity *type_entity_new();

void type_entity_dtor(Type_entity *);

size_t type_entity_sz(const size_t name_ln);