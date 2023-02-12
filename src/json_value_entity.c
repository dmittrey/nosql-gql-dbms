#include "entity/json_value_entity.h"

json_value_entity *json_value_entity_new()
{
    return my_malloc(json_value_entity);
}

void json_value_entity_ctor(json_value_entity *json_entity, json_value_t *json)
{
    json_entity->attr_count = json->object.attributes_count;
    json_entity->type = json->type;
    json_entity->val_ptr = 0;
    json_entity->parent = 0;
    json_entity->next = 0;
}
void json_value_entity_dtor(json_value_entity *json_entity)
{
    free(json_entity);
}

size_t json_value_entity_get_size(json_value_entity *json)
{
    return sizeof(json->attr_count) +
           sizeof(json->type) +
           sizeof(json->val_ptr) +
           sizeof(json->parent) +
           sizeof(json->next) +
           json->attr_count * sizeof(kv_entity);
}