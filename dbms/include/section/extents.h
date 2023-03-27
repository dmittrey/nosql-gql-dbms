#pragma once

#include "utils/utils.h"
#include "utils/table.h"
#include "utils/string.h"

#include "json/entity.h"
#include "json/pair.h"

#include "query/query.h"

#include "section/header.h"

struct Sect_ext;

struct Sect_ext *sect_ext_new();

Status sect_ext_ctor(struct Sect_ext *const, const Fileoff, FILE *const);
Status sect_ext_dtor(struct Sect_ext *);

Status sect_ext_write(struct Sect_ext *const section, const String *const key, const void *const data, const size_t data_sz, Entity *const entity, Sectoff *const save_addr);
Status sect_ext_read(const struct Sect_ext *const section, const Sectoff entity_addr, Entity *const o_entity, Json *const o_json);
Status sect_ext_update(struct Sect_ext *const section, const Sectoff soff, const String *const new_key, const void *const val, const size_t val_sz, Entity *const entity);
Status sect_ext_delete(struct Sect_ext *const section, const Sectoff sectoff, Entity *del_entity);
Status sect_ext_load(const struct Sect_ext *const section, List_Fileoff_itm *const collection);

typedef struct
{
    Sect_head_entity header;
} Sect_ext_entity;