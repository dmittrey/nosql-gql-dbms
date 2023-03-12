#pragma once

#include "memory/type/type.h"

// Контракт (Все атрибуты в той же секции)

typedef struct
{
    size_t name_size;
    sectoff_t name_ptr;
    size_t attr_cnt;
} type_entity_t;

type_entity_t *type_entity_new();

void type_entity_dtor(type_entity_t *);

size_t type_entity_sz(const size_t name_ln);