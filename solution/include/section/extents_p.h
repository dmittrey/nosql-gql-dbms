#pragma once

#include "utils/pair.h"

#include "section/header_p.h"

#include "json/pair.h"

typedef struct Sect_ext
{
    Sect_head header;
    struct Sect_ext *next;
} Sect_ext;

Status sect_ext_wrt_itm(Sect_ext *const section, const Sectoff sectoff, const Entity *const entity);
Status sect_ext_rd_itm(const Sect_ext *const section, const Sectoff sectoff, Entity *const o_entity);

Status sect_ext_load(const Sect_ext *const section, List_Pair_Json_Entity *const collection);

int sect_ext_cmp(Sect_ext *this, Sect_ext *other);

LIST_DECLARE(Sect_ext);