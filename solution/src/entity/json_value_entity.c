#include <string.h>

#include "entity/json_value_entity.h"

json_value_entity *json_value_entity_new()
{
    return memset(my_malloc(json_value_entity), 0, sizeof(json_value_entity));
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