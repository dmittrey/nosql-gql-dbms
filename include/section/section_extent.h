#pragma once

#include "utils.h"
#include "table.h"

#include "xml/xml_node.h"

#include "section/section_page.h"

typedef struct
{
    section_page_t page;
} section_extent_t;

section_extent_t *section_extent_new();

void section_extent_ctor(section_extent_t *, FILE *, fileoff_t);
void section_extent_dtor(section_extent_t *);

PerformStatus section_extent_sync(section_extent_t *);

PerformStatus section_extent_write(section_extent_t *, xml_node_t *);