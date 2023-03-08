#pragma once

#include "memory/attribute/attr.h"

typedef struct attr_col_t
{
    attr_t *f_attr;
    attr_t *l_attr;
    size_t count;
} attr_col_t;

attr_col_t *attr_col_new();

void attr_col_dtor(attr_col_t *);

void attr_col_add_nxt(attr_col_t *const, attr_t *const);

void attr_col_del_fst(attr_col_t *const);
void attr_col_del_nxt(attr_col_t *const, attr_t *const);