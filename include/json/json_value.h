#pragma once

#include "utils.h"
#include "table.h"

#include "json/json_object.h"

typedef enum
{
    TYPE_INT32 = 0,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_OBJECT
} json_value_type;

typedef struct
{
    json_object_t object; // Указатель на массив атрибутов Nullable
    union
    {
        int32_t int32_val;
        float float_val;
        string_t string_val;
        bool bool_val;
    } value;
    json_value_type type; // Тип значения
} json_value_t;

json_value_t *json_value_new();

void json_value_ctor(json_value_t *, json_value_type, uint64_t);
void json_value_dtor(json_value_t *);

sectoff_t json_value_get_item_size(json_value_t *);
sectoff_t json_value_get_record_size(json_value_t *);

int32_t json_value_get_int32(json_value_t *);
float json_value_get_float(json_value_t *);
string_t json_value_get_string(json_value_t *);
bool json_value_get_bool(json_value_t *);