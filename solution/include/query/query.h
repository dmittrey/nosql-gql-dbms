#pragma once

#include "query/query_item.h"

typedef struct Query
{
    String *type_name;
    Query_item *f_query_itm;
} Query;

Query *query_new();
void query_ctor(Query *, String *);

void query_dtor(Query *);

void query_item_add(Query *const, Query_item *const);

bool query_check(const Query *const q, const Json *const json, const Type *json_type);