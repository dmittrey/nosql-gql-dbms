#pragma once

#include "utils.h"
#include "table.h"

#include "json/json_value.h"

#include "section/section_header.h"

typedef struct
{
    section_header_t header;
} section_extents_t;

section_extents_t *section_extents_new();

void section_extents_ctor(section_extents_t *, fileoff_t, FILE *);
void section_extents_dtor(section_extents_t *);

PerformStatus section_extents_sync(section_extents_t *);

PerformStatus section_extents_write(section_extents_t *, json_value_t *, fileoff_t *);
PerformStatus section_extents_read(section_extents_t *, json_value_t *);
PerformStatus section_extents_update(section_extents_t *, json_value_t *);
PerformStatus section_extents_delete(section_extents_t *, json_value_t *);