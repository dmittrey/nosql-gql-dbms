#pragma once

#include "utils.h"

#include "memory/json/json.h"

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
} entity_t;

entity_t *entity_new();

void entity_ctor(entity_t *const entity, const json_t *const json, const fileoff_t dad_json_addr, const fileoff_t bro_json_addr, const fileoff_t son_json_addr);
void entity_dtor(entity_t *);
