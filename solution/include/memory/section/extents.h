#pragma once

#include "utils.h"
#include "table.h"

#include "memory/section/header.h"

#include "memory/json/json.h"
#include "physical/json/entity.h"

typedef struct
{
    sect_head_t header;
} sect_ext_t;

sect_ext_t *sect_ext_new();

status_t sect_ext_ctor(sect_ext_t *const, const fileoff_t, FILE *const);
void sect_ext_dtor(sect_ext_t *);

status_t sect_ext_write(sect_ext_t *const section, const json_t *const json, const tplgy_addr tplgy_addrs, sectoff_t *const save_addr);
status_t sect_ext_read(const sect_ext_t *const section, const sectoff_t entity_addr, entity_t *const o_entity, json_t *const json);
status_t sect_ext_update(sect_ext_t *const, const sectoff_t, const json_t *const);
status_t sect_ext_delete(sect_ext_t *const, const sectoff_t);

status_t sect_ext_sync(sect_ext_t *const);