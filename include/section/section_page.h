#pragma once

#include "utils.h"
#include "table.h"

#define SECTION_PAGE_FIELD_SETTER_DECLARE(FIELD_TYPE, FIELD_NAME) \
    PerformStatus section_page_set_##FIELD_NAME(section_page_t *page, FIELD_TYPE FIELD_NAME)

typedef struct
{
    sectoff_t free_space;
    sectoff_t next;             // Nullable
    sectoff_t last_item_ptr;    // Pointer to first free cell(after items)
    sectoff_t first_record_ptr; // Pointer to last free cell(before records)
    fileoff_t section_offset;
    FILE *filp;
} section_page_t;

section_page_t *section_page_new();

void section_page_ctor(section_page_t *, fileoff_t, FILE *);
void section_page_dtor(section_page_t *);

PerformStatus section_page_shift_last_item_ptr(section_page_t *, sectoff_t);
PerformStatus section_page_shift_first_record_ptr(section_page_t *, sectoff_t);

PerformStatus section_page_sync(section_page_t *);

sectoff_t section_page_size();

SECTION_PAGE_FIELD_SETTER_DECLARE(sectoff_t, free_space);
SECTION_PAGE_FIELD_SETTER_DECLARE(sectoff_t, next);
SECTION_PAGE_FIELD_SETTER_DECLARE(sectoff_t, last_item_ptr);
SECTION_PAGE_FIELD_SETTER_DECLARE(sectoff_t, first_record_ptr);