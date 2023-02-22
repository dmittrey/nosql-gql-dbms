#pragma once

#include "utils.h"

#include "json/json_value.h"

typedef struct
{
    sectoff_t key_ptr;
    size_t key_size;
    sectoff_t val_ptr;
    size_t val_size;
    fileoff_t dad_ptr;
    fileoff_t bro_ptr;
    fileoff_t son_ptr;
    uint64_t type;
} json_value_entity_t;

json_value_entity_t *json_value_entity_new();

void json_value_entity_ctor(json_value_entity_t *const json_entity, const json_value_t *const json, const fileoff_t parent_json_addr, const fileoff_t bro_json_addr, const fileoff_t son_json_addr);
void json_value_entity_dtor(json_value_entity_t *);

size_t json_value_entity_get_physical_size(const json_value_entity_t *const);
