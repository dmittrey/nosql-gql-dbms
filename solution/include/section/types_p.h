#pragma once

#include "utils/pair.h"

#include "section/header_p.h"
#include "section/types.h"

#include "json/object/attr_ent.h"
#include "json/object/type_ent.h"

typedef struct Sect_types
{
    Sect_head header;
    struct Sect_types *next;
} Sect_types;

PAIR_DECLARE(Sectoff, Type);
LIST_DECLARE(Pair_Sectoff_Type);

Status sect_types_load(Sect_types *const section, List_Pair_Sectoff_Type *const o_list);

Status sect_types_rd_type(Sect_types *const section, const Sectoff sectoff, Type_entity *const o_ent);
Status sect_types_rd_atr(Sect_types *const section, const Sectoff sectoff, Attr_entity *const o_atr);

Status sect_types_wrt_type(Sect_types *const section, const Sectoff sectoff, const Type_entity *const o_ent);
Status sect_types_wrt_atr(Sect_types *const section, const Sectoff sectoff, const Attr_entity *const o_atr);

int sect_types_cmp(Sect_types * this, Sect_types * other);

LIST_DECLARE(Sect_types);