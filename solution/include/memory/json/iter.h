#pragma once

#include "memory/file/file.h"

typedef struct
{
    file_t *file;
    query_t *query;
    sect_ext_t *cur_sect;
    list_json_t *cur_json_col;
    bool is_loaded_cur_sect;
} iter_t;

iter_t *iter_new();

void iter_ctor(iter_t *const, file_t *, query_t *);
void iter_dtor(iter_t *);

status_t iter_next(iter_t *const);
json_t *iter_get(iter_t *const);
