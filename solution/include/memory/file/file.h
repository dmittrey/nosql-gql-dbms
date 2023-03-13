#pragma once

#include "memory/section/extents.h"
#include "memory/section/types.h"
#include "physical/section/extents.h"
#include "physical/section/types.h"

#include "physical/file/header.h"

#include "memory/type/type.h"

typedef struct
{
    file_head_t header;
    sect_ext_t *f_extent;
    sect_type_t *f_types;
    FILE *filp;
} file_t;

file_t *file_new();

void file_ctor(file_t *const, FILE *const);
void file_dtor(file_t *);

status_t file_add_type(file_t *const file, const type_t* const type);
status_t file_delete_type(file_t *const file, const string_t *const name);
status_t file_find_type(file_t *const file, const string_t *const name, type_t *const o_type, sect_type_t **o_types);

status_t file_write(file_t *const file, const json_t *const json, fileoff_t dad_fileoff, fileoff_t *const write_addr);
status_t file_read(file_t *const file, const fileoff_t fileoff, json_t *const ret_json);
status_t file_update(file_t *const file, const fileoff_t fileoff, const json_t *const new_json, const fileoff_t dad_ptr, bool is_bro_upd, fileoff_t *cur_fileoff);
status_t file_delete(file_t *const file, const fileoff_t fileoff, bool is_root);
status_t file_find(file_t *const file, const query_t *const query, list_json_t* const o_obj_col);

status_t file_add_sect_ext(file_t *const file, sect_ext_t *r_extents);
status_t file_add_sect_types(file_t *const file, sect_type_t *r_types);