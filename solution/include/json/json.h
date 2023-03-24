#pragma once

#include "utils/utils.h"
#include "utils/table.h"
#include "utils/string.h"

#include "json/object/type.h"

#define JSON_VALUE_INIT_TYPE_INT32(JSON_NAME, KEY, VAL) \
    Json *JSON_NAME = json_new();                       \
    json_ctor(JSON_NAME, TYPE_INT32, KEY, strlen(KEY)); \
    JSON_NAME->value.int32_val = VAL;

#define JSON_VALUE_INIT_TYPE_FLOAT(JSON_NAME, KEY, VAL) \
    Json *JSON_NAME = json_new();                       \
    json_ctor(JSON_NAME, TYPE_FLOAT, KEY, strlen(KEY)); \
    JSON_NAME->value.float_val = VAL

#define JSON_VALUE_INIT_TYPE_STRING(JSON_NAME, KEY, VAL) \
    Json *JSON_NAME = json_new();                        \
    json_ctor(JSON_NAME, TYPE_STRING, KEY, strlen(KEY)); \
    JSON_NAME->value.string_val = VAL

#define JSON_VALUE_INIT_TYPE_BOOL(JSON_NAME, KEY, VAL) \
    Json *JSON_NAME = json_new();                      \
    json_ctor(JSON_NAME, TYPE_BOOL, KEY, strlen(KEY)); \
    JSON_NAME->value.bool_val = VAL

#define JSON_VALUE_INIT_TYPE_OBJECT(JSON_NAME, KEY, VAL) \
    Json *JSON_NAME = json_new();                        \
    json_ctor(JSON_NAME, TYPE_OBJECT, KEY, strlen(KEY))

#define JSON_VALUE_INIT(TYPE, JSON_NAME, KEY, VAL) \
    JSON_VALUE_INIT_##TYPE(JSON_NAME, KEY, VAL)

typedef struct Json
{
    String *key;
    union
    {
        int32_t int32_val;
        float float_val;
        String *string_val;
        bool bool_val;
    } value;
    struct Json *dad;
    struct Json *bro; // next json with same parent
    struct Json *son;
    uint64_t type;     // Тип значения
    struct Json *next; // Указатель на следующий json в секции
    Fileoff foff;
} Json;

Json *json_new();

void json_ctor(Json *const, const Json_type, const char *const, const size_t);
void json_dtor(Json *);
void json_dtor_with_bro(Json *json);

void *json_val_ptr(const Json *const);
size_t json_val_size(const Json *const);

void json_add_bro(Json *const j, Json *const bro);
void json_add_son(Json *const j, Json *const son);
void json_add_nxt(Json *const j, Json *const nxt);

int json_cmp(const Json *const this, const Json *const other);
int json_cmp_wth_foff(const Json *const this, const Json *const other);

bool json_is_apply_type(const Json *const json, const Type *const type);

Json* json_son_by_key(const Json *const json, String* key);