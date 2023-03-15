#pragma once

#include "memory/section/header_p.h"

typedef struct sect_type_t
{
    sect_head_t header;
    struct sect_type_t *next;
} sect_type_t;