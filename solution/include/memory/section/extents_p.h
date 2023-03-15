#pragma once

#include "memory/section/header_p.h"

#include "memory/json/json.h"

typedef struct sect_ext_t
{
    struct sect_head_t header;
    json_t *f_json; // TODO Need refactor???
    struct sect_ext_t *next;
} sect_ext_t;