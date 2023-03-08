#pragma once

#include "memory/json/json.h"

typedef struct attr_t
{
    json_type_t type;
    struct attr_t *next;
} attr_t;

attr_t *attr_new();

void attr_ctor(attr_t *const, const json_type_t);
void attr_dtor(attr_t *);