#pragma once

#include "utils/utils.h"
#include "utils/table.h"

#include "json/object/attr.h"

typedef struct
{
    size_t name_size;
    Sectoff name_ptr;
    uint64_t attr_type;
} Attr_entity;

Attr_entity *attr_entity_new();

void attr_entity_dtor(Attr_entity *);

size_t attr_entity_sz(const size_t name_ln);