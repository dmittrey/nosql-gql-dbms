#pragma once

#include "memory/section/header.h"

#include "memory/type/type.h"
#include "physical/type/type.h"
#include "physical/type/attr.h"

struct sect_type_t *sect_type_new();

status_t sect_type_ctor(struct sect_type_t *const, const fileoff_t, FILE *const);
void sect_type_dtor(struct sect_type_t *);

status_t sect_type_write(struct sect_type_t *const section, const type_t *const type, sectoff_t *const o_wrt_soff);
status_t sect_type_delete(struct sect_type_t *const section, const sectoff_t del_soff);
status_t sect_type_find(struct sect_type_t *const section, const string_t *const type_name, type_t *const o_type);

status_t sect_types_sync(struct sect_type_t *const);

status_t sect_type_read(struct sect_type_t *const section, sectoff_t sctoff, type_t *const o_type, type_entity_t *const o_type_ent);
status_t sect_type_load(struct sect_type_t *const section, list_type_t *const o_type_list);

status_t sect_type_rd_ent(struct sect_type_t *const section, const sectoff_t sectoff, type_entity_t *const o_ent);
status_t sect_type_rd_atr(struct sect_type_t *const section, const sectoff_t sectoff, attr_entity_t *const o_atr);