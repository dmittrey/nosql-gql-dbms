#pragma once

#include "memory/section/extents.h"

typedef struct
{
    sect_ext_t *extents;
} file_t;

file_t *file_new();

void file_ctor(file_t *const, FILE *const);
status_t file_dtor(file_t *);

status_t file_write(file_t *const file, const json_t *const json);
status_t file_read(file_t *const file, const char *path, json_t *const ret_json);
status_t file_update(file_t *const file, const char *path, const json_t *const new_json);
status_t file_delete(file_t *const file, const char *path);

sect_ext_t *get_sect_ext(fileoff_t);