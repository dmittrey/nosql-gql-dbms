#pragma once

#include "memory/json/json.h"

typedef struct
{
    json_t *f_json;
    json_t *l_json;
    size_t count;
} json_col_t;

json_col_t *json_col_new();

void json_col_ctor(json_col_t *const, json_col_t *);
void json_col_dtor(json_col_t *);

void json_col_add(json_col_t *, json_t *);
void json_col_del_nxt(json_col_t *, json_t *);
void json_col_del_fst(json_col_t *);

// Add to end with comparing like set write(N) XD GLHF
void json_col_add_lk_set(json_col_t *, json_t *);