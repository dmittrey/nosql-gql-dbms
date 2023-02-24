#include <string.h>

#include "physical/json/entity.h"

entity_t *entity_new()
{
    return memset(my_malloc(entity_t), 0, sizeof(entity_t));
}

void entity_ctor(entity_t *const entity, const json_t *const json, const fileoff_t dad_json_addr, const fileoff_t bro_json_addr, const fileoff_t son_json_addr)
{
    entity->key_ptr = 0;
    entity->key_size = json->key.cnt;
    entity->val_ptr = 0;
    entity->val_size = json_val_size(json);
    entity->dad_ptr = dad_json_addr;
    entity->bro_ptr = bro_json_addr;
    entity->son_ptr = son_json_addr;
    entity->type = json->type;
}
void entity_dtor(entity_t *entity)
{
    free(entity);
}