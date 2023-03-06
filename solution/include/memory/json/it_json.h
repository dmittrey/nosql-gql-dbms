#pragma once

#include "memory/json/json.h"

// Iterable json abstraction
typedef struct
{
    json_t *json;
    struct it_json_t *next;
} it_json_t;

it_json_t *it_json_new();

void it_json_ctor(it_json_t *const it_json, json_t * json);
void it_json_dtor(it_json_t *);

void it_json_add_nxt(it_json_t *const json, it_json_t * next);