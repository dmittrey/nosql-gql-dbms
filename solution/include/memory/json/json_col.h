#pragma once

#include "memory/json/json.h"

typedef struct
{
    json_t * f_json;
    size_t count;
} json_col_t;

json_col_t *json_col_new();

void json_col_dtor(json_col_t *);

void json_col_add(json_col_t *, json_t *);