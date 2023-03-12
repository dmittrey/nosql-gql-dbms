#pragma once

#include "memory/type/attr.h"

typedef struct
{
    size_t name_size;
    sectoff_t name_ptr;
    uint64_t attr_type;
} attr_entity_t;

attr_entity_t *attr_entity_new();

void attr_entity_ctor(attr_entity_t *const, const attr_t *const);
void attr_entity_dtor(attr_entity_t *);

size_t attr_entity_sz(const size_t name_ln);