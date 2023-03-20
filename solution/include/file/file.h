#pragma once

#include "file/header.h"

#include "section/extents.h"
#include "section/types.h"

struct File;

struct File *file_new();

Status file_ctor(struct File *const, FILE *const);
Status file_dtor(struct File *);

Status file_add_type(struct File *const file, const Type *const type, Fileoff *wrt_foff);
Status file_delete_type(struct File *const file, const String *const name);
Status file_find_type(struct File *const file, const String *const name, Type *const o_type, Sectoff *const o_adr, struct Sect_types **o_sect_ptr);

Status file_write(struct File *const file, const Json *const json, Fileoff dad_fileoff, Fileoff type_foff, Fileoff *const wrt_foff);
Status file_read(struct File *const file, const Fileoff fileoff, Json *const ret_json, Entity* const ret_entity);
Status file_delete(struct File *const file, const Fileoff fileoff, const bool is_root);
Status file_update(struct File *const file, const Fileoff fileoff, const Json *const new_json, const Fileoff dad_ptr, const Fileoff type_ptr, bool is_root, Fileoff *cur_fileoff);
Status file_find(struct File *const file, struct Sect_ext *section, const Query *const query, List_Pair_Json_Entity *const o_obj_col);