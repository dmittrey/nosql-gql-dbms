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

struct json_value_t;

typedef struct
{
    struct
    {
        uint64_t attributes_count;
        struct json_kv_t **attributes;
    } object; // Указатель на массив атрибутов Nullable
    union
    {
        int32_t int32_val;
        float float_val;
        string_t string_val;
        bool bool_val;
    } value;
    size_t type; // Тип значения
} json_value_t;

json_value_t *json_value_new();

void json_value_ctor(json_value_t * const, const json_value_type, const uint64_t);
void json_value_dtor(json_value_t *);

void json_value_print(const json_value_t * const);

size_t json_value_get_serialization_size(const json_value_t * const);

struct json_kv_t
{
    string_t key;
    json_value_t *value;
};

struct json_kv_t *json_kv_new();

void json_kv_ctor(struct json_kv_t * const, const string_t, json_value_t*);
void json_kv_dtor(struct json_kv_t *);