#pragma once

#include "utils.h"
#include "table.h"

#include "memory/section/header.h"

#include "physical/type/type.h"
#include "physical/type/attr.h"

typedef struct sect_ext_t
{
    sect_head_t header;
    struct sect_ext_t *next;
} sect_type_t;

sect_type_t *sect_type_new();

status_t sect_type_ctor(sect_type_t *const, const fileoff_t, FILE *const);
void sect_type_dtor(sect_type_t *);

status_t sect_type_write(sect_type_t *const section, const type_t *const type, sectoff_t *const o_wrt_soff);
status_t sect_type_delete(sect_type_t *const section, const sectoff_t del_soff);
status_t sect_type_find(sect_type_t *const section, string_t *const type_name, type_t *const o_type);

status_t sect_type_read(sect_type_t *const section, sectoff_t sctoff, type_t *const o_type, type_entity_t *const o_type_ent);
status_t sect_type_load(sect_type_t *const section, list_type_t *const o_type_list);

status_t sect_type_rd_ent(sect_type_t *const section, const sectoff_t sectoff, type_entity_t *const o_ent);
status_t sect_type_rd_atr(sect_type_t *const section, const sectoff_t sectoff, attr_entity_t *const o_atr);