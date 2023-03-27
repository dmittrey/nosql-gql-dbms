#pragma once

#include "utils/table.h"

#include "json/object/attr.h"

#define TYPE_INIT(VAR_NAME, NAME)                          \
    Type *VAR_NAME = type_new();                           \
    STR_INIT(VAR_NAME##_type_name, NAME);                  \
    List_Attr *VAR_NAME##_attr_list = list_Attr_new(); \
    type_ctor(VAR_NAME, VAR_NAME##_type_name, VAR_NAME##_attr_list);

typedef struct Type
{
    String *name;
    List_Attr *attr_list;
    struct Type *next;
} Type;

Type *type_new();

void type_ctor(Type *const, String *name, List_Attr *const attr_list);
void type_dtor(Type *);

int type_cmp(const Type *const t1, const Type *const t2);
void type_cpy(Type *const dest, Type *const src);

void type_add_atr(Type *const, Attr *const);

size_t type_ph_sz(const Type *const type);
size_t type_itm_sz(const Type *const type);
size_t type_rec_sz(const Type *const type);

LIST_DECLARE(Type);
