#pragma once

#include "utils.h"
#include "table.h"

#include "memory/section/header.h"

#include "memory/query/query.h"

#include "physical/json/entity.h"

typedef struct sect_ext_t
{
    sect_head_t header;
    json_t *f_json;
    struct sect_ext_t *next;
} sect_type_t;