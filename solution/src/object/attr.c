#include "json/object/attr.h"

Attr *attr_new()
{
    return calloc(1, sizeof(Attr));
}

void attr_ctor(Attr *const attr, String *const name, const Json_type type)
{
    attr->name = name;
    attr->type = type;
}

void attr_dtor(Attr *attr)
{
    string_dtor(attr->name);
    free(attr);
}

int attr_cmp(Attr *const a1, Attr *const a2)
{
    int str_cmp = string_cmp(a1->name, a2->name);

    if (str_cmp == 0)
    {
        return a1->type != a2->type;
    }
    else
    {
        return str_cmp;
    }
}

LIST_DEFINE(Attr, attr_dtor, attr_cmp);

