#include "json/entity.h"

Entity *entity_new()
{
    return my_calloc(Entity);
}

void entity_ctor(Entity *const entity, const size_t key_sz, const size_t val_sz, const uint64_t type, const Tplgy* const tplgy)
{
    entity->key_size = key_sz;
    entity->val_size = val_sz;
    entity->fam_addr = *tplgy;
    entity->type = type;
    entity->init_rec_sz = key_sz + val_sz;
}
void entity_dtor(Entity *entity)
{
    free(entity);
}

size_t entity_itm_size(const Entity *const ent)
{
    return sizeof(Entity);
}

size_t entity_rec_sz(const Entity *const entity)
{
    return entity->key_size + entity->val_size;
}

size_t entity_init_rec_sz(const Entity *const entity)
{
    return entity->init_rec_sz;
}

size_t entity_ph_size(const Entity *const ent)
{
    return entity_itm_size(ent) + entity_rec_sz(ent);
}

Entity *entity_clear(Entity *const e)
{
    return memset(e, 0, sizeof(Entity));
}

Entity *entity_cpy(Entity *const dest, Entity *const src)
{
    return memcpy(dest, src, sizeof(Entity));
}

int entity_cmp(Entity *const e1, Entity *const e2)
{
    return memcmp(e1, e2, sizeof(Entity));
}