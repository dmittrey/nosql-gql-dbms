#pragma once

#include "utils/utils.h"
#include "utils/string.h"

#include "json/json.h"
#include "json/type.h"

#define QUERY_ITEM_INIT_TYPE_INT32(QUERY_NAME, KEY, VAL) \
    Query_item *QUERY_NAME = query_item_new();           \
    query_item_ctor(QUERY_NAME, TYPE_INT32, KEY);        \
    QUERY_NAME->query_val.int32_val = VAL;

#define QUERY_ITEM_INIT_TYPE_FLOAT(QUERY_NAME, KEY, VAL) \
    Query_item *QUERY_NAME = query_item_new();           \
    query_item_ctor(QUERY_NAME, TYPE_FLOAT, KEY);        \
    QUERY_NAME->query_val.float_val = VAL

#define QUERY_ITEM_INIT_TYPE_STRING(QUERY_NAME, KEY, VAL) \
    Query_item *QUERY_NAME = query_item_new();            \
    query_item_ctor(QUERY_NAME, TYPE_STRING, KEY);        \
    QUERY_NAME->query_val.string_val = VAL

#define QUERY_ITEM_INIT_TYPE_BOOL(QUERY_NAME, KEY, VAL) \
    Query_item *QUERY_NAME = query_item_new();          \
    query_item_ctor(QUERY_NAME, TYPE_BOOL, KEY);        \
    QUERY_NAME->query_val.bool_val = VAL

#define QUERY_ITEM_INIT(TYPE, QUERY_NAME, KEY, VAL) \
    QUERY_ITEM_INIT_##TYPE(QUERY_NAME, KEY, VAL)

typedef struct Query_item
{
    String *query_key;
    union
    {
        int32_t int32_val;
        float float_val;
        String *string_val;
        bool bool_val;
    } query_val;
    Json_type type;
    struct Query_item *next;
} Query_item;

Query_item *query_item_new();

void query_item_ctor(Query_item *const, const Json_type, String *key);
void query_item_dtor(Query_item *);

bool query_item_check(const Query_item *const, const Json *const);