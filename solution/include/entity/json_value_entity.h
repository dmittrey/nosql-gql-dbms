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
} json_value_entity;

json_value_entity *json_value_entity_new();

void json_value_entity_ctor(json_value_entity *, json_value_t *);
void json_value_entity_dtor(json_value_entity *);
