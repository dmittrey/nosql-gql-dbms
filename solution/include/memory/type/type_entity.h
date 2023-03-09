#pragma once

#include "memory/type/type.h"

typedef struct {
    fileoff_t next_ptr;
    size_t name_size;
    sectoff_t name_ptr;
    sectoff_t f_attr_ptr; // Контракт (Все атрибуты в той же секции)
} type_entity;