#pragma once

#include "memory/section/header.h"

#include "memory/query/query.h"

#include "physical/json/entity.h"

struct sect_ext_t *sect_ext_new();

status_t sect_ext_ctor(struct sect_ext_t *const, const fileoff_t, FILE *const);
void sect_ext_dtor(struct sect_ext_t *);

status_t sect_ext_wrt_itm(struct sect_ext_t *const, const sectoff_t, const entity_t *const);
status_t sect_ext_rd_itm(const struct sect_ext_t *const, const sectoff_t, entity_t *const o_entity);
status_t sect_ext_wrt_rec(struct sect_ext_t *const, const sectoff_t, const void *const, const size_t);
status_t sect_ext_rd_rec(const struct sect_ext_t *const, const sectoff_t, const size_t, void *const o_val);

status_t sect_ext_write(struct sect_ext_t *const section, const json_t *const json, entity_t *const entity, sectoff_t *const save_addr);
status_t sect_ext_read(const struct sect_ext_t *const section, const sectoff_t entity_addr, entity_t *const o_entity, json_t *const o_json);
status_t sect_ext_update(struct sect_ext_t *const section, const sectoff_t sectoff, const json_t *const new_json, entity_t *const new_entity);
status_t sect_ext_delete(struct sect_ext_t *const section, const sectoff_t sectoff, entity_t *del_entity);
status_t sect_ext_find(struct sect_ext_t *const section, const query_t* const query, list_json_t *const o_json_col);

status_t sect_ext_sync(struct sect_ext_t *const);

status_t sect_ext_load(const struct sect_ext_t *const section, list_json_t *const collection);
status_t sect_ext_add_next(struct sect_ext_t *const section, struct sect_ext_t *const);