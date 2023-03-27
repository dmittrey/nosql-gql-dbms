#pragma once

#include "utils/table.h"
#include "utils/string.h"

#include "json/object/type.h"
#include "json/object/type_ent.h"

#include "section/header.h"

struct Sect_types;

struct Sect_types *sect_types_new();

Status sect_types_ctor(struct Sect_types *const, const Fileoff, FILE *const);
Status sect_types_dtor(struct Sect_types *);

Status sect_types_write(struct Sect_types *const section, const Type *const type, Sectoff *const o_wrt_soff);
Status sect_types_delete(struct Sect_types *const section, const Sectoff del_soff);
Status sect_types_read(struct Sect_types *const section, Sectoff sctoff, Type *const o_type, Type_entity *const o_type_ent, size_t *o_ph_sz);

Status sect_types_find(struct Sect_types *const section, const String *const type_name, Type *const o_type, Sectoff *adr);