#pragma once

#include "memory/section/extents.h"
#include "physical/section/extents.h"

typedef struct
{
    fileoff_t lst_sect_ptr;
    sect_ext_t *f_extent;
    FILE *filp;
} file_t;

file_t *file_new();

void file_ctor(file_t *const, FILE *const);
void file_dtor(file_t *);

status_t file_write(file_t *const file, const json_t *const json, fileoff_t dad_fileoff, const fileoff_t *write_addr);
status_t file_read(file_t *const file, const fileoff_t fileoff, json_t *const ret_json);
status_t file_update(file_t *const file, const fileoff_t fileoff, const json_t *const new_json);
status_t file_delete(file_t *const file, const fileoff_t fileoff);

status_t file_add_sect_ext(file_t *const file, sect_ext_t *r_extents);
status_t file_read_sect_ext(file_t *const, const fileoff_t, sect_ext_t *const);