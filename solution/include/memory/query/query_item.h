#pragma once

#include "utils.h"
#include "string.h"

#include "memory/json/json.h"

#define QUERY_ITEM_INIT_TYPE_INT32(QUERY_NAME, KEY, VAL) \
    query_item_t *QUERY_NAME = query_item_new();         \
    query_item_ctor(QUERY_NAME, TYPE_INT32, KEY);        \
    QUERY_NAME->query_val.int32_val = VAL;

#define QUERY_ITEM_INIT_TYPE_FLOAT(QUERY_NAME, KEY, VAL) \
    query_item_t *QUERY_NAME = query_item_new();         \
    query_item_ctor(QUERY_NAME, TYPE_FLOAT, KEY);        \
    QUERY_NAME->query_val.float_val = VAL

#define QUERY_ITEM_INIT_TYPE_STRING(QUERY_NAME, KEY, VAL) \
    query_item_t *QUERY_NAME = query_item_new();          \
    query_item_ctor(QUERY_NAME, TYPE_STRING, KEY);        \
    QUERY_NAME->query_val.string_val = VAL

#define QUERY_ITEM_INIT_TYPE_BOOL(QUERY_NAME, KEY, VAL) \
    query_item_t *QUERY_NAME = query_item_new();        \
    query_item_ctor(QUERY_NAME, TYPE_BOOL, KEY);        \
    QUERY_NAME->query_val.bool_val = VAL

#define QUERY_ITEM_INIT(TYPE, QUERY_NAME, KEY, VAL) \
    QUERY_ITEM_INIT_##TYPE(QUERY_NAME, KEY, VAL)

typedef struct query_item_t
{
    string_t *query_key;
    union
    {
        int32_t int32_val;
        float float_val;
        string_t *string_val;
        bool bool_val;
    } query_val;
    json_type_t type;
    struct query_item_t *next;
} query_item_t;

query_item_t *query_item_new();

void query_item_ctor(query_item_t *const, const json_type_t, string_t * key);
void query_item_dtor(query_item_t *);

bool query_item_check(const query_item_t *const, const json_t *const);