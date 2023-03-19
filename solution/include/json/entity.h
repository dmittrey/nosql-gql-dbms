#pragma once

#include "utils/utils.h"
#include "utils/table.h"

#include "json/json.h"

#define TPLGY_INIT(NAME, DAD_ADDR, BRO_ADDR, SON_ADDR) \
    Tplgy *NAME = my_calloc(Tplgy);                    \
    NAME->dad_ptr = DAD_ADDR;                          \
    NAME->bro_ptr = BRO_ADDR;                          \
    NAME->son_ptr = SON_ADDR

#define ENTITY_INIT(ENTITY_NAME, JSON_NAME, DAD, BRO, SON, TYPE_PTR) \
    Entity *ENTITY_NAME = entity_new();                              \
    TPLGY_INIT(TPLGY_##ENTITY_NAME, DAD, BRO, SON);                  \
    ENTITY_NAME->type_ptr = TYPE_PTR;                                \
    entity_ctor(ENTITY_NAME, JSON_NAME->key->cnt, json_val_size(JSON_NAME), JSON_NAME->type, TPLGY_##ENTITY_NAME)

typedef struct Tplgy
{
    Fileoff dad_ptr;
    Fileoff bro_ptr;
    Fileoff son_ptr;
} Tplgy;

typedef struct Entity
{
    Sectoff key_ptr;
    size_t key_size;
    Sectoff val_ptr;
    size_t val_size;
    uint64_t type;
    size_t init_rec_sz; // Size of initial entity record(after write)
    Tplgy fam_addr;
    Fileoff type_ptr;
} Entity;

Entity *entity_new();

void entity_ctor(Entity *const entity, const size_t key_sz, const size_t val_sz,
                 const uint64_t type, const Tplgy *const tplgy);
void entity_dtor(Entity *);

size_t entity_itm_size(const Entity *const entity);
size_t entity_rec_sz(const Entity *const entity);
size_t entity_init_rec_sz(const Entity *const entity);
size_t entity_ph_size(const Entity *const entity);

Entity *entity_clear(Entity *const);
Entity *entity_cpy(Entity *const dest, Entity *const src);

int entity_cmp(Entity *const e1, Entity *const e2);
