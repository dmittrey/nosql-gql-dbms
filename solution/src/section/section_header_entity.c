#include "physical/section/header.h"

sect_head_entity_t *sect_head_entity_new()
{
    return my_malloc(sect_head_entity_t);
}

void sect_head_entity_dtor(sect_head_entity_t *section)
{
    free(section);
}

