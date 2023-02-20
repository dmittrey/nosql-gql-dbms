#include <string.h>

#include "table.h"

#include "json/json_value.h"
#include "entity/json_value_entity.h"

json_value_t *json_value_new()
{
    return memset(my_malloc(json_value_t), 0, sizeof(json_value_t));
}

void json_value_ctor(json_value_t * const json, const json_value_type type, const uint64_t attributes_count)
{
    json->object.attributes_count = attributes_count;
    if (json->object.attributes_count != 0)
    {
        json->object.attributes = my_malloc_array(struct json_kv_t *, attributes_count);
    }
    json->type = type;
}

void json_value_dtor(json_value_t *json)
{
    for (size_t i = 0; i < json->object.attributes_count; i++)
    {
        json_kv_dtor(json->object.attributes[i]);
    }

    if (json->object.attributes_count != 0)
    {
        free(json->object.attributes);
    }

    if (json->type == TYPE_STRING)
    {
        string_dtor(&json->value.string_val);
    }

    free(json);
}

void json_value_print(const json_value_t * const json)
{
    switch (json->type)
    {
    case TYPE_STRING:
        printf("%s ", json->value.string_val.val);
        break;
    case TYPE_INT32:
        printf("%d ", json->value.int32_val);
        break;
    case TYPE_FLOAT:
        printf("%.6f ", json->value.float_val);
        break;
    case TYPE_BOOL:
        printf("%d ", json->value.bool_val);
        break;
    default:
        break;
    }

    if (json->object.attributes_count == 0)
    {
        return;
    }

    printf("{");
    for (size_t i = 0; i < json->object.attributes_count; i++)
    {
        printf("\n\"%s\" : ", json->object.attributes[i]->key.val);
        json_value_print(json->object.attributes[i]->value);
    }
    printf("\n}");
}

size_t json_value_get_serialization_size(const json_value_t * const json)
{
    size_t size = sizeof(json_value_entity);

    if (json->type == TYPE_STRING)
    {
        size += string_get_size(json->value.string_val);
    }
    else if (json->type != TYPE_OBJECT)
    {
        size += sizeof(json->value);
    }

    size += json->object.attributes_count * sizeof(attr_entity);

    for (size_t i = 0; i < json->object.attributes_count; i++)
    {
        size += string_get_size(json->object.attributes[i]->key);
        size += json_value_get_serialization_size(json->object.attributes[i]->value);
    }

    return size;
}

struct json_kv_t *json_kv_new()
{
    return my_malloc(struct json_kv_t);
}

void json_kv_ctor(struct json_kv_t * const json_kv, const string_t string, json_value_t *json_val)
{
    json_kv->key = string;
    json_kv->value = json_val;
}

void json_kv_dtor(struct json_kv_t *json_kv) {
    string_dtor(&json_kv->key);
    json_value_dtor(json_kv->value);
    free(json_kv);
}