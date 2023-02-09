#include <stdio.h>
#include <assert.h>

#include "utils.h"
#include "table.h"

#include "section/section_header.h"

#define SECTION_HEADER_FIELD_SETTER_DEFINE(FIELD_TYPE, FIELD_NAME)                               \
    PerformStatus section_header_set_##FIELD_NAME(section_header_t *page, FIELD_TYPE FIELD_NAME) \
    {                                                                                            \
        page->FIELD_NAME = FIELD_NAME;                                                           \
        FWRITE_OR_FAIL(&page->FIELD_NAME, sizeof(FIELD_TYPE), page->filp);                       \
        return OK;                                                                               \
    }

/**
 * Write inner information into file
 */
PerformStatus section_header_sync(section_header_t *header)
{
    long prev_ptr = ftell(header->filp);

    FWRITE_OR_FAIL(&header->free_space, sizeof(header->free_space), header->filp);
    FWRITE_OR_FAIL(&header->next, sizeof(header->next), header->filp);
    FWRITE_OR_FAIL(&header->last_item_ptr, sizeof(header->last_item_ptr), header->filp);
    FWRITE_OR_FAIL(&header->first_record_ptr, sizeof(header->first_record_ptr), header->filp);

    FSEEK_OR_FAIL(header->filp, prev_ptr);

    return OK;
}

sectoff_t section_header_size(section_header_t *header)
{
    return sizeof(header->free_space) + sizeof(header->next) + sizeof(header->last_item_ptr) + sizeof(header->first_record_ptr);
}

section_header_t *section_header_new()
{
    return my_malloc(section_header_t);
}

void section_header_ctor(section_header_t *header, fileoff_t offset, FILE *filp)
{
    assert(filp != NULL);

    header->free_space = SECTION_SIZE - section_page_size(header);
    header->next = 0; // If we have 0 then we don't have next section
    header->last_item_ptr = ftell(filp) + section_page_size(header);
    header->first_record_ptr = ftell(filp) + SECTION_SIZE;
    header->section_offset = offset;
    header->filp = filp;

    section_header_sync(header);
}
void section_header_dtor(section_header_t *header)
{
    free(header);
}

PerformStatus section_header_shift_last_item_ptr(section_header_t *header, sectoff_t shift)
{
    header->free_space -= shift;
    header->last_item_ptr += shift;

    section_header_sync(header);

    return OK;
}

PerformStatus section_header_shift_first_record_ptr(section_header_t *header, sectoff_t shift)
{
    header->free_space -= shift;
    header->first_record_ptr += shift;

    section_header_sync(header);

    return OK;
}

SECTION_HEADER_FIELD_SETTER_DEFINE(sectoff_t, free_space);
SECTION_HEADER_FIELD_SETTER_DEFINE(sectoff_t, next);
SECTION_HEADER_FIELD_SETTER_DEFINE(sectoff_t, last_item_ptr);
SECTION_HEADER_FIELD_SETTER_DEFINE(sectoff_t, first_record_ptr);