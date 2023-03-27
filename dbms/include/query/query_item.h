#pragma once

#include "utils/utils.h"
#include "utils/string.h"

#include "json/json.h"
#include "json/type.h"

#include "query/path.h"

typedef struct Query_item
{
    List_Path *key;
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

void query_item_ctor(Query_item *const, const Json_type, List_Path *key);
void query_item_dtor(Query_item *);

bool query_item_check(const Query_item *const, const Json *const);