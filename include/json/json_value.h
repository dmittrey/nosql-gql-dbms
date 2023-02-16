#pragma once

#include "utils.h"
#include "table.h"

typedef enum
{
    TYPE_INT32 = 0,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_OBJECT,
    TYPE_ARRAY
} json_value_type;

struct kv;

typedef struct
{
    struct
    {
        uint64_t attributes_count;
        struct kv **attributes;
    } object; // Указатель на массив атрибутов Nullable
    // json_object_t *arr_objects;
    union
    {
        int32_t int32_val;
        float float_val;
        string_t string_val;
        bool bool_val;
    } value;
    json_value_type type; // Тип значения
} json_value_t;

struct kv
{
    string_t key;
    json_value_t *value;
};

json_value_t *json_value_new();

void json_value_ctor(json_value_t *, json_value_type, uint64_t);
void json_value_dtor(json_value_t *);

sectoff_t json_value_get_item_size(json_value_t *);
sectoff_t json_value_get_record_size(json_value_t *);

void json_value_print(json_value_t *);