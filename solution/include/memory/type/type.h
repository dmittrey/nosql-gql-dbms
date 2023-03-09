#pragma once

#include "memory/type/attr.h"

typedef struct
{
    string_t *name;
    list_attr_t *attr_list;
} type_t;

type_t *type_new();

void type_ctor(type_t *const, string_t* name, list_attr_t* const attr_list);
void type_dtor(type_t *);

