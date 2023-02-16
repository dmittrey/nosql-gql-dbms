#pragma once

#include "utils.h"
#include "table.h"

#define SECTION_HEADER_FIELD_SETTER_DECLARE(FIELD_TYPE, FIELD_NAME) \
    PerformStatus section_header_set_##FIELD_NAME(section_header_t *page, FIELD_TYPE FIELD_NAME)

typedef struct
{
    sectoff_t free_space;
    fileoff_t next;             // Nullable
    sectoff_t last_item_ptr;    // Pointer to first free cell(after items)
    sectoff_t first_record_ptr; // Pointer to last free cell(before records)
    fileoff_t section_offset;
    FILE *filp;
} section_header_t;

section_header_t *section_header_new();

void section_header_ctor(section_header_t *, fileoff_t, FILE *);
void section_header_dtor(section_header_t *);

PerformStatus section_header_shift_last_item_ptr(section_header_t *, sectoff_t);
PerformStatus section_header_shift_first_record_ptr(section_header_t *, sectoff_t);

PerformStatus section_header_sync(section_header_t *);

sectoff_t section_header_size();

SECTION_HEADER_FIELD_SETTER_DECLARE(sectoff_t, free_space);
SECTION_HEADER_FIELD_SETTER_DECLARE(sectoff_t, next);
SECTION_HEADER_FIELD_SETTER_DECLARE(sectoff_t, last_item_ptr);
SECTION_HEADER_FIELD_SETTER_DECLARE(sectoff_t, first_record_ptr);