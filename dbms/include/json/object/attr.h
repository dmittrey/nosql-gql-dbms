#pragma once

#include "utils/string.h"
#include "utils/list.h"

#include "json/type.h"

#define ATR_INIT(VAR_NAME, NAME, TYPE)   \
    Attr *VAR_NAME = attr_new();         \
    STR_INIT(VAR_NAME##_atr_name, NAME); \
    attr_ctor(VAR_NAME, VAR_NAME##_atr_name, TYPE);

typedef struct Attr
{
    String *name;
    Json_type type;
    struct Attr *next;
} Attr;

Attr *attr_new();

void attr_ctor(Attr *const, String *const, const Json_type);
void attr_dtor(Attr *);

int attr_cmp(Attr *const, Attr *const);

LIST_DECLARE(Attr);