#include <string.h>

#include "table.h"

#include "json/json_value.h"
#include "entity/json_value_entity.h"

json_value_t *json_value_new()
{
    return memset(my_malloc(json_value_t), 0, sizeof(json_value_t));
}

void json_value_ctor(json_value_t *const json, const json_value_type type, const char *const json_key, const size_t json_key_count)
{
    string_ctor(&json->key, json_key, json_key_count);

    memset(&json->value, 0, sizeof(json->value));

    json->dad = NULL;
    json->bro = NULL;
    json->son = NULL;

    json->type = type;
}

void json_value_dtor(json_value_t *json)
{
    // Change dad's son ptr to json bro
    if (json->dad != NULL)
    {
        json->dad->son = json->bro;
    }

    // Free all children
    struct json_value_t *cur_son = json->son;
    while (cur_son != NULL)
    {
        free(cur_son);
        cur_son = cur_son->bro;
    }

    // Free json
    string_dtor(&json->key);
    if (json->type == TYPE_STRING)
    {
        string_dtor(&json->value.string_val);
    }
    free(json);
}

void *json_value_get_val_ptr(const json_value_t *const json)
{
    if (json->type == TYPE_OBJECT)
    {
        return NULL;
    }
    else if (json->type == TYPE_STRING)
    {
        return json->value.string_val.val;
    }
    else
    {
        return (void *)&json->value;
    }
}

size_t json_value_get_val_size(const json_value_t *const json)
{
    switch (json->type)
    {
    case TYPE_OBJECT:
        return 0;
    case TYPE_STRING:
        return string_get_size(json->value.string_val);
    default:
        return sizeof(json->value);
    }
}

void json_value_add_bro(json_value_t *const json, json_value_t *bro)
{
    struct json_value_t *cur_json = json;
    while (cur_json->bro != NULL)
    {
        cur_json = cur_json->bro;
    }

    cur_json->bro = bro;
}
void json_value_add_son(json_value_t *const json, json_value_t *son)
{
    if (json->son == NULL)
    {
        json->son = son;
    }
    else
    {
        struct json_value_t *cur_son = json->son;
        while (cur_son->bro != NULL)
        {
            cur_son = cur_son->bro;
        }

        cur_son->bro = son;
    }
}

void json_value_print(const json_value_t *const json)
{
    printf("%s ", json->key.val);

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
        printf("{");

        struct json_value_t *cur_son = json->son;
        while (cur_son != NULL)
        {
            json_value_print(cur_son);
            cur_son = cur_son->bro;
        }

        printf("\n}");
        break;
    }
}