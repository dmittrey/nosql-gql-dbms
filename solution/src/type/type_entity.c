#include "physical/type/type.h"
#include "physical/type/attr.h"

type_entity_t *type_entity_new()
{
    return calloc(1, sizeof(type_entity_t));
}

void type_entity_dtor(type_entity_t *ent)
{
    free(ent);
}

size_t type_entity_sz(const size_t name_ln)
{
    return sizeof(type_entity_t) +  name_ln * sizeof(char);
}