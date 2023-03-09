#pragma once

#include "list.h"

#include "memory/json/json.h"

typedef struct attr_t
{
    string_t *name;
    json_type_t type;
    struct attr_t *next;
} attr_t;

attr_t *attr_new();

void attr_ctor(attr_t *const, const json_type_t);
void attr_dtor(attr_t *);

int attr_cmp(attr_t *const, attr_t *const);

LIST_DECLARE(attr_t);