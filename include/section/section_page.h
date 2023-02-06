#pragma once

#include "utils.h"
#include "table.h"

typedef struct
{
    sectoff_t free_space;
    sectoff_t next;             // Nullable
    sectoff_t last_item_ptr;    // Pointer to first free cell(after items)
    sectoff_t first_record_ptr; // Pointer to last free cell(before records)
    fileoff_t fileoff;
    FILE *filp;
} section_page_t;

section_page_t *section_page_new();

void section_page_ctor(section_page_t *, FILE *, fileoff_t);
void section_page_dtor(section_page_t *);

PerformStatus section_page_sync(section_page_t *);