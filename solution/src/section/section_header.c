#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "utils.h"
#include "table.h"

#include "section/section_header.h"

/**
 * Write inner information into file
 */
PerformStatus section_header_sync(section_header_t *const header)
{
    long prev_ptr = ftell(header->filp);
    RANDOM_ACCESS_FWRITE_OR_FAIL(header, section_header_size(), header->section_offset, header->filp);
    FSEEK_OR_FAIL(header->filp, prev_ptr);

    return OK;
}

sectoff_t section_header_size()
{
    return sizeof(section_header_t) - sizeof(FILE *) - sizeof(fileoff_t); // - section_offset - filp
}

section_header_t *section_header_new()
{
    return memset(my_malloc(section_header_t), 0, sizeof(section_header_t));
}

void section_header_ctor(section_header_t *const header, const fileoff_t offset, FILE *const filp)
{
    assert(filp != NULL);

    header->free_space = SECTION_SIZE - section_header_size();
    header->next = 0; // If we have 0 then we don't have next section
    header->last_item_ptr = offset + section_header_size();
    header->first_record_ptr = offset + SECTION_SIZE;
    header->section_offset = offset;
    header->filp = filp;

    section_header_sync(header);
}
void section_header_dtor(section_header_t *header)
{
    free(header);
}

PerformStatus section_header_shift_last_item_ptr(section_header_t *const header, const int64_t shift)
{
    header->free_space -= shift;
    header->last_item_ptr += shift;

    section_header_sync(header);

    return OK;
}

PerformStatus section_header_shift_first_record_ptr(section_header_t *const header, const int64_t shift)
{
    header->free_space += shift;
    header->first_record_ptr += shift;

    section_header_sync(header);

    return OK;
}