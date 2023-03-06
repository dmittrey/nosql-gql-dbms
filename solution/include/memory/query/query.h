#pragma once

#include "utils.h"
#include "string.h"

#include "memory/json/json.h"

#include "memory/query/query_item.h"

typedef struct
{
    query_item_t *f_query_itm;
} query_t;

query_t *query_new();

void query_dtor(query_t *);

void query_item_add(query_t *const, query_item_t *const);

bool query_check_or(const query_t *const, const json_t *const);
bool query_check_and(const query_t *const, const json_t *const);