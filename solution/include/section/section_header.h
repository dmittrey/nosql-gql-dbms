#pragma once

#include "utils.h"
#include "table.h"

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

void section_header_ctor(section_header_t * const, const fileoff_t, FILE * const);
void section_header_dtor(section_header_t *);

PerformStatus section_header_shift_last_item_ptr(section_header_t * const, const sectoff_t);
PerformStatus section_header_shift_first_record_ptr(section_header_t * const, const sectoff_t);

PerformStatus section_header_sync(section_header_t * const);

sectoff_t section_header_size();