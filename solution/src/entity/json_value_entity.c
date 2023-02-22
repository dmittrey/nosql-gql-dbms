#include <string.h>

#include "entity/json_value_entity.h"

json_value_entity_t *json_value_entity_new()
{
    return memset(my_malloc(json_value_entity_t), 0, sizeof(json_value_entity_t));
}

void json_value_entity_ctor(json_value_entity_t *const json_entity, const json_value_t *const json, const fileoff_t parent_json_addr, const fileoff_t bro_json_addr, const fileoff_t son_json_addr)
{
    json_entity->key_ptr = 0;
    json_entity->key_size = json->key.count;
    json_entity->val_ptr = 0;
    json_entity->val_size = json_value_get_val_size(json);
    json_entity->dad_ptr = parent_json_addr;
    json_entity->bro_ptr = bro_json_addr;
    json_entity->son_ptr = son_json_addr;
    json_entity->type = json->type;
}
void json_value_entity_dtor(json_value_entity_t *json_entity)
{
    free(json_entity);
}

size_t json_value_entity_get_physical_size(const json_value_entity_t *const entity)
{
    return sizeof(entity) + entity->key_size + entity->val_size;
}