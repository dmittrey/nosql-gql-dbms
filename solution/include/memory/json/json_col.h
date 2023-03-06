#pragma once

#include "memory/json/json.h"

typedef struct
{
    json_t *pointer;
} iter_t;

iter_t *iter_new();

void iter_dtor(iter_t *);

typedef struct
{
    json_t *f_json;
    json_t *l_json;
} json_col_t;

json_col_t *json_col_new();

void json_col_ctor(json_col_t *const, json_col_t *);
void json_col_dtor(json_col_t *);

void json_col_add(json_col_t *, json_t *);

iter_t json_col_begin();
iter_t json_col_end();