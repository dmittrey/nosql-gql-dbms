#pragma once

#include "memory/file/file.h"

typedef struct
{
    file_t *file;
    query_t *query;
    sect_ext_t *cur_sect;
    json_col_t *cur_json_col;
} iter_t;

iter_t *iter_new();

void iter_ctor(iter_t *const, const file_t *const, const query_t *const);
void iter_dtor(iter_t *);

status_t iter_next(iter_t *const);
json_t *iter_get(iter_t *const);
