#pragma once

#include "utils.h"

#include "json/json_value.h"

#include "entity/json_object_entity.h"

typedef struct
{
    uint64_t attr_count;
    uint64_t type;
    uint64_t val_size;
    sectoff_t val_ptr;
    kv_entity attributes[0];
} json_value_entity;

typedef struct
{
    json_object_t object; // Указатель на массив атрибутов
    union
    {
        int32_t int32_val;
        float float_val;
        string_t string_val;
        bool bool_val;
    } value;
    json_value_type type; // Тип значения
} json_value_t_entity;