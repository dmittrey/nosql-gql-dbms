#pragma once

#include "memory/type/attr.h"

typedef struct {
    sectoff_t next_ptr;
    size_t name_size;
    sectoff_t name_ptr;
    uint64_t attr_type;
} attr_entity;