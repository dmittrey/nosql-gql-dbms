#include "json/object/type_ent.h"

Type_entity *type_entity_new()
{
    return my_calloc(Type_entity);
}

void type_entity_dtor(Type_entity *ent)
{
    free(ent);
}

size_t type_entity_sz(const size_t name_ln)
{
    return sizeof(Type_entity) + name_ln * sizeof(char);
}