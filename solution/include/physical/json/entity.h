#pragma once

#include "utils.h"

#include "memory/json/json.h"

#define ENTITY_INIT(ENTITY_NAME, JSON_NAME, DAD_ADDR, BRO_ADDR, SON_ADDR) \
    entity_t *ENTITY_NAME = entity_new();                                 \
    entity_ctor(ENTITY_NAME, JSON_NAME, DAD_ADDR, BRO_ADDR, SON_ADDR);

typedef struct
{
    fileoff_t dad_ptr;
    fileoff_t bro_ptr;
    fileoff_t son_ptr;
} tplgy_addr;

typedef struct
{
    sectoff_t key_ptr;
    size_t key_size;
    sectoff_t val_ptr;
    size_t val_size;
    uint64_t type;
    size_t rec_size;
    tplgy_addr fam_addr;
} entity_t;

entity_t *entity_new();

void entity_ctor(entity_t *const entity, const json_t *const json, const fileoff_t dad_json_addr, const fileoff_t bro_json_addr, const fileoff_t son_json_addr);
void entity_dtor(entity_t *);

size_t entity_itm_size(const entity_t *const entity);
// Returns true entity rec size
size_t entity_rec_size(const entity_t *const entity);
// Returns cell for rec size(rec may be not equals val + key sizes after update)
size_t entity_rec_pool_size(const entity_t *const entity);
size_t entity_ph_size(const entity_t *const entity);

entity_t *entity_clear(entity_t *const);
entity_t *entity_cpy(entity_t *const dest, entity_t *const src);

int entity_cmp(entity_t *const e1, entity_t *const e2);
