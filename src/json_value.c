#include "table.h"

#include "json/json_value.h"

#define ATTRIBUTE_ITEM_SIZE 24
#define VALUE_ITEM_SIZE 16

json_value_t *json_value_new()
{
    return my_malloc(json_value_t);
}

void json_value_dtor(json_value_t *json)
{
    for (size_t i = 0; i < json->object.attributes_count; i++)
    {
        free(json->object.attributes[i]->key.val);
        json_value_dtor(json->object.attributes[i]->value);
    }
    free(json->object.attributes);

    free(json->value.string_val.val);

    free(json);
}

sectoff_t json_value_get_item_size(json_value_t *json)
{
    return (json->object.attributes_count * ATTRIBUTE_ITEM_SIZE) + sizeof(json->type) + VALUE_ITEM_SIZE;
}

sectoff_t json_value_get_record_size(json_value_t *json)
{
    sectoff_t record_size = 0;

    for (size_t i = 0; i < json->object.attributes_count; i++)
    {
        record_size += string_get_size(json->object.attributes[i]->key);
    }
    if (json->type == TYPE_STRING)
    {
        record_size += string_get_size(json->value.string_val);
    }

    return record_size;
}