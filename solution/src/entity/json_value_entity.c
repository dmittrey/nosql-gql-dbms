#include <string.h>

#include "entity/json_value_entity.h"

json_value_entity *json_value_entity_new()
{
    return memset(my_malloc(json_value_entity), 0, sizeof(json_value_entity));
}

void json_value_entity_ctor(json_value_entity *json_entity, json_value_t *json)
{
    json_entity->key_ptr = NULL;
    json_entity->key_size = json->key.count;
    json_entity->val_ptr = NULL;
    json_entity->val_size = json_value_get_val_size(json);
    json_entity->dad_ptr = NULL;
    json_entity->bro_ptr = NULL;
    json_entity->son_ptr = NULL;
    json_entity->type = json->type;
}
void json_value_entity_dtor(json_value_entity *json_entity)
{
    free(json_entity);
}