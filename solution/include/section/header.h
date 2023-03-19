#pragma once

#include "utils/utils.h"
#include "utils/table.h"

struct Sect_head;

struct Sect_head *sect_head_new();

Status sect_head_ctor(struct Sect_head *const, const Fileoff, FILE *const);
Status sect_head_dtor(struct Sect_head *);

Status sect_head_sync(struct Sect_head *const);

Status sect_head_shift_lip(struct Sect_head *const, const size_t);
Status sect_head_shift_frp(struct Sect_head *const, const size_t);

Status sect_head_cmprs_lip(struct Sect_head *const, const size_t);
Status sect_head_cmprs_frp(struct Sect_head *const);

Status sect_head_read(struct Sect_head *const header, const Sectoff soff, const size_t sz, void *const o_data);
Status sect_head_write(struct Sect_head *const, const Sectoff, const size_t, const void *const);

Fileoff sect_head_get_fileoff(const struct Sect_head *const, const Sectoff);
Sectoff sect_head_get_sectoff(const struct Sect_head *const, const Fileoff);

typedef struct
{
    Sectoff free_space;
    Fileoff next_ptr;
    Sectoff lst_itm_ptr;
    Sectoff fst_rec_ptr;
} Sect_head_entity;