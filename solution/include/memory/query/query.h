#pragma once

#include "utils.h"
#include "string.h"

#include "memory/json/json.h"

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
    struct query_item_t *next;
} query_item_t;

bool query_item_check(const query_item_t *const, const json_t *const);

typedef struct
{
    query_item_t *f_query_itm;
} query_t;

bool query_check(const query_t *const, const json_t *const);