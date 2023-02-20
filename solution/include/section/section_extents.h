#pragma once

#include "utils.h"
#include "table.h"

#include "json/json_value.h"

#include "section/section_header.h"

#define SECTION_EXTENTS_ITEM_SIZE 8

typedef struct
{
    section_header_t header;
} section_extents_t;

section_extents_t *section_extents_new();

void section_extents_ctor(section_extents_t *const, const fileoff_t, FILE *const);
PerformStatus section_extents_dtor(section_extents_t *);

PerformStatus section_extents_sync(section_extents_t *const);

PerformStatus section_extents_write(section_extents_t *const, const json_value_t *const, const fileoff_t *const parent_json_addr, fileoff_t *const save_json_addr);
PerformStatus section_extents_read(const section_extents_t *const, const sectoff_t, json_value_t *const);
PerformStatus section_extents_update(section_extents_t *const, const sectoff_t, const json_value_t *const);
PerformStatus section_extents_delete(const section_extents_t *const, const sectoff_t);