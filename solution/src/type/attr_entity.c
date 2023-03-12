#include "physical/type/attr.h"

attr_entity_t *attr_entity_new()
{
    return calloc(1, sizeof(attr_entity_t));
}

void attr_entity_ctor(attr_entity_t *const ent, const attr_t *const atr)
{
    ent->name_size = atr->name->cnt;
    ent->attr_type = atr->type;
}

void attr_entity_dtor(attr_entity_t *ent)
{
    free(ent);
}

size_t attr_entity_sz(const size_t name_ln)
{
    return sizeof(attr_entity_t) + name_ln * sizeof(char);
}