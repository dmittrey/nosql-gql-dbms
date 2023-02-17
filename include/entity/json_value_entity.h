#pragma once

#include "utils.h"

#include "json/json_value.h"

#include "entity/json_object_entity.h"

// #define JSON_VALUE_ENTITY_GETTER_DECL(TYPE, FIELD) \
    // TYPE json_value_entity_get_##FIELD(json_value_entity *)

typedef struct
{
    uint64_t attr_count;
    uint64_t type;
    uint64_t val_size;
    sectoff_t val_ptr;
    fileoff_t parent;  
    fileoff_t next;
    uint64_t size; // Size of serialized struct
} json_value_entity;

json_value_entity *json_value_entity_new();

void json_value_entity_ctor(json_value_entity *, json_value_t *);
void json_value_entity_dtor(json_value_entity *);

uint64_t json_value_entity_get_attr_count(json_value_entity *);

// JSON_VALUE_ENTITY_GETTER_DECL(uint64_t, attr_count);
