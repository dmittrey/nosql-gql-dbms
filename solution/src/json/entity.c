#include <string.h>

#include "physical/json/entity.h"

entity_t *entity_new()
{
    return memset(my_malloc(entity_t), 0, sizeof(entity_t));
}

void entity_ctor(entity_t *const entity, const json_t *const json, const fileoff_t dad_json_addr, const fileoff_t bro_json_addr, const fileoff_t son_json_addr)
{
    entity->key_ptr = 0;
    entity->key_size = json->key->cnt;
    entity->val_ptr = 0;
    entity->val_size = json_val_size(json);
    entity->fam_addr.dad_ptr = dad_json_addr;
    entity->fam_addr.bro_ptr = bro_json_addr;
    entity->fam_addr.son_ptr = son_json_addr;
    entity->type = json->type;
    entity->rec_size = entity->key_size + entity->val_size;
}
void entity_dtor(entity_t *entity)
{
    free(entity);
}

size_t entity_itm_size(const entity_t *const entity)
{
    return sizeof(entity_t);
}

size_t entity_rec_size(const entity_t *const entity)
{
    return entity->key_size + entity->val_size;
}

size_t entity_ph_size(const entity_t *const entity)
{
    return entity_itm_size(entity) + entity_rec_size(entity);
}

entity_t *entity_clear(entity_t *const entity)
{
    return memset(entity, 0, sizeof(entity_t));
}

entity_t *entity_cpy(entity_t *const dest, entity_t *const src)
{
    return memcpy(dest, src, sizeof(entity_t));
}