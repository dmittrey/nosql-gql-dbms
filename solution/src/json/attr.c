#include "memory/attribute/attr.h"

attr_t *attr_new()
{
    return calloc(1, sizeof(attr_t));
}

void attr_ctor(attr_t *const attr, const json_type_t type)
{
    attr->type = type;
}

void attr_dtor(attr_t *attr)
{
    free(attr);
}