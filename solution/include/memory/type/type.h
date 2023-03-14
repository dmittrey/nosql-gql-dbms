#pragma once

#include "memory/type/attr.h"

#define TYPE_INIT(VAR_NAME, NAME)                          \
    type_t *VAR_NAME = type_new();                         \
    STR_INIT(VAR_NAME##_type_name, NAME);                  \
    list_attr_t *VAR_NAME##_attr_list = list_attr_t_new(); \
    type_ctor(VAR_NAME, VAR_NAME##_type_name, VAR_NAME##_attr_list);

typedef struct type_t
{
    string_t *name;
    list_attr_t *attr_list;
    struct type_t *next;
    fileoff_t foff_ptr;
} type_t;

type_t *type_new();

void type_ctor(type_t *const, string_t *name, list_attr_t *const attr_list);
void type_ctor_foff(type_t *const, string_t *name, list_attr_t *const attr_list, sectoff_t foff);
void type_dtor(type_t *);

int type_cmp(const type_t *const t1, const type_t *const t2);
void type_cpy(type_t *const dest, type_t *const src);

void type_add_atr(type_t *const, attr_t *const);

size_t type_ph_sz(const type_t *const type);

LIST_DECLARE(type_t);
