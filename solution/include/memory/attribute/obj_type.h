#pragma once

#include "memory/attribute/attr_col.h"

typedef struct
{
    string_t *key;
    attr_col_t *attrs;
} obj_type_t;

obj_type_t *obj_type_new();

void obj_type_ctor(obj_type_t *const, string_t *const, attr_col_t *const);
void obj_type_dtor(obj_type_t *);

int obj_type_cmp(obj_type_t *const, json_t *const);