#pragma once

#include "utils.h"
#include "table.h"

typedef struct
{
    sectoff_t free_space;
    fileoff_t next_ptr;    // Nullable
    sectoff_t lst_itm_ptr; // Pointer to first free cell(after items)
    sectoff_t fst_rec_ptr; // Pointer to last free cell(before records)
    fileoff_t sect_off;
    FILE *filp;
} sect_head_t;

sect_head_t *sect_head_new();

status_t sect_head_ctor(sect_head_t *const, const fileoff_t, FILE *const);
void sect_head_dtor(sect_head_t *);

status_t sect_head_shift_lst_itm_ptr(sect_head_t *const, const int64_t);
status_t sect_head_shift_fst_rec_ptr(sect_head_t *const, const int64_t);

status_t sect_head_sync(sect_head_t *const);

fileoff_t sect_head_get_fileoff(const sect_head_t *const, const sectoff_t);