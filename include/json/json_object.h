#pragma once

#include "utils.h"

struct json_value_t;

typedef struct
{
    string_t key;
    struct json_value_t *value;
} kv;

typedef struct
{
    uint64_t attributes_count;
    kv **attributes;
} json_object_t;