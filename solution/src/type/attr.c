#include "memory/type/attr.h"

attr_t *attr_new()
{
    return calloc(1, sizeof(attr_t));
}

void attr_ctor(attr_t *const attr, string_t *const name, const json_type_t type)
{
    attr->name = name;
    attr->type = type;
}

void attr_dtor(attr_t *attr)
{
    free(attr);
}

int attr_cmp(attr_t *const a1, attr_t *const a2)
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

LIST_DEFINE(attr_t, attr_dtor, attr_cmp);