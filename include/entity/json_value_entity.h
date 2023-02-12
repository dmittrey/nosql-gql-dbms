#pragma once

#include "utils.h"

#include "json/json_value.h"

#include "entity/json_object_entity.h"

typedef struct
{
    uint64_t attr_count;
    uint64_t type;
    sectoff_t val_ptr;
    fileoff_t parent;
    fileoff_t next;
} json_value_entity;

json_value_entity *json_value_entity_new();

void json_value_entity_ctor(json_value_entity *, json_value_t *);
void json_value_entity_dtor(json_value_entity *);

size_t json_value_entity_get_size(json_value_entity *);