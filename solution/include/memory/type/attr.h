#pragma once

#include "utils.h"
#include "table.h"
#include "list.h"

#include "memory/string.h"

#include "memory/json/json_type.h"

#define ATR_INIT(VAR_NAME, NAME, TYPE)         \
    attr_t *VAR_NAME = attr_new();       \
    STR_INIT(VAR_NAME##_atr_name, NAME); \
    attr_ctor(VAR_NAME, VAR_NAME##_atr_name, TYPE);

typedef struct attr_t
{
    string_t *name; // TODO Убрать имя
    json_type_t type;
    struct attr_t *next;
} attr_t;

attr_t *attr_new();

void attr_ctor(attr_t *const, string_t *const, const json_type_t);
void attr_dtor(attr_t *);

int attr_cmp(attr_t *const, attr_t *const);

LIST_DECLARE(attr_t);