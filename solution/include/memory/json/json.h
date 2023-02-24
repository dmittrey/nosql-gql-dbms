#pragma once

#include "utils.h"
#include "table.h"

#define JSON_VALUE_INIT_TYPE_INT32(JSON_NAME, KEY, VAL)       \
    json_t *JSON_NAME = json_new();               \
    json_ctor(JSON_NAME, TYPE_INT32, KEY, strlen(KEY)); \
    JSON_NAME->value.int32_val = VAL;

#define JSON_VALUE_INIT_TYPE_FLOAT(JSON_NAME, KEY, VAL)       \
    json_t *JSON_NAME = json_new();               \
    json_ctor(JSON_NAME, TYPE_FLOAT, KEY, strlen(KEY)); \
    JSON_NAME->value.float_val = VAL

#define JSON_VALUE_INIT_TYPE_STRING(JSON_NAME, KEY, VAL)       \
    json_t *JSON_NAME = json_new();                \
    json_ctor(JSON_NAME, TYPE_STRING, KEY, strlen(KEY)); \
    JSON_NAME->value.string_val = VAL

#define JSON_VALUE_INIT_TYPE_BOOL(JSON_NAME, KEY, VAL)       \
    json_t *JSON_NAME = json_new();              \
    json_ctor(JSON_NAME, TYPE_BOOL, KEY, strlen(KEY)); \
    JSON_NAME->value.bool_val = VAL

#define JSON_VALUE_INIT_TYPE_OBJECT(JSON_NAME, KEY, VAL) \
    json_t *JSON_NAME = json_new();          \
    json_ctor(JSON_NAME, TYPE_OBJECT, KEY, strlen(KEY))

#define JSON_VALUE_INIT(TYPE, JSON_NAME, KEY, VAL) \
    JSON_VALUE_INIT_##TYPE(JSON_NAME, KEY, VAL)

typedef enum
{
    TYPE_INT32 = 0,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_OBJECT,
    TYPE_ARRAY
} json_type_t;

typedef struct json_t
{
    string_t *key;
    union
    {
        int32_t int32_val;
        float float_val;
        string_t *string_val;
        bool bool_val;
    } value;
    struct json_t *dad;
    struct json_t *bro; // next json with same parent
    struct json_t *son;
    uint64_t type; // Тип значения
} json_t;

json_t *json_new();

void json_ctor(json_t *const, const json_type_t, const char *const, const size_t);
void json_dtor(json_t *);

void *json_val_ptr(const json_t *const);
size_t json_val_size(const json_t *const);

void json_add_bro(json_t *const json, json_t *bro);
void json_add_son(json_t *const json, json_t *son);

void json_print(const json_t *const);