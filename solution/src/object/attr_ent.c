#include "json/object/attr.h"
#include "json/object/attr_ent.h"

Attr_entity *attr_entity_new()
{
    return my_calloc(Attr_entity);
}

void attr_entity_dtor(Attr_entity *ent)
{
    free(ent);
}

size_t attr_entity_sz(const size_t name_ln)
{
    return sizeof(Attr_entity) + name_ln * sizeof(char);
}