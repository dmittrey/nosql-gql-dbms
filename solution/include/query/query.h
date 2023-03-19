#pragma once

#include "query/query_item.h"

typedef struct Query
{
    Query_item *f_query_itm;
} Query;

Query *query_new();

void query_dtor(Query *);

void query_item_add(Query *const, Query_item *const);

bool query_check_or(const Query *const, const Json *const);
bool query_check_and(const Query *const, const Json *const);