#include <string.h>

#include "table.h"

#include "memory/json/json.h"

#include "physical/json/entity.h"

json_t *json_new()
{
    return memset(my_malloc(json_t), 0, sizeof(json_t));
}

void json_ctor(json_t *const json, const json_type_t type, const char *const key_val, const size_t key_size)
{
    string_ctor(&json->key, key_val, key_size);

    memset(&json->value, 0, sizeof(sizeof(json_t)));

    json->dad = NULL;
    json->bro = NULL;
    json->son = NULL;

    json->type = type;
}

void json_dtor(json_t *json)
{
    // Change dad's son ptr to json bro
    if (json->dad != NULL)
    {
        json->dad->son = json->bro;
    }

    // Free all children
    json_t *cur_son = json->son;
    while (cur_son != NULL)
    {
        free(cur_son);
        cur_son = cur_son->bro;
    }

    // Free json
    string_dtor(&json->key);
    if (json->type == TYPE_STRING)
    {
        string_dtor(json->value.string_val);
    }
    free(json);
}

void *json_val_ptr(const json_t *const json)
{
    switch (json->type)
    {
    case TYPE_OBJECT:
        return NULL;
    case TYPE_STRING:
        return json->value.string_val->val;
    default:
        return (void *)&json->value;
    }
}

size_t json_val_size(const json_t *const json)
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

void json_add_bro(json_t *const json, json_t *bro)
{
    json_t *cur_json = json;
    while (cur_json->bro != NULL)
    {
        cur_json = cur_json->bro;
    }

    cur_json->bro = bro;
}

void json_add_son(json_t *const json, json_t *son)
{
    if (json->son == NULL)
    {
        json->son = son;
    }
    else
    {
        json_t *cur_son = json->son;
        while (cur_son->bro != NULL)
        {
            cur_son = cur_son->bro;
        }

        cur_son->bro = son;
    }
}

void json_print(const json_t *const json)
{
    printf("%s ", json->key.val);

    switch (json->type)
    {
    case TYPE_STRING:
        printf("%s ", json->value.string_val->val);
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

        json_t *cur_son = json->son;
        while (cur_son != NULL)
        {
            json_print(cur_son);
            cur_son = cur_son->bro;
        }

        printf("\n}");
        break;
    }
}