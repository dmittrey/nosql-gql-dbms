#pragma once

#include "utils/utils.h"
#include "utils/table.h"

struct File_head;

struct File_head *file_head_new();

Status file_head_ctor(struct File_head *const, FILE *const);
Status file_head_dtor(struct File_head *);

Status file_head_read(struct File_head *const header, const Fileoff foff, const size_t sz, void *const o_data);
Status file_head_write(struct File_head *const header, const Fileoff foff, const size_t sz, const void *const data);

Status file_head_sync(struct File_head *const);

Status file_head_shift_lsp(struct File_head *const, const size_t); // Last section ptr

typedef struct File_head_entity
{
    Fileoff lst_sect_ptr;
} File_head_entity;