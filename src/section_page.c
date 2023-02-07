#include <stdio.h>
#include <assert.h>

#include "utils.h"
#include "table.h"

#include "section/section_page.h"

#define SECTION_PAGE_FIELD_SETTER_DEFINE(FIELD_TYPE, FIELD_NAME)                             \
    PerformStatus section_page_set_##FIELD_NAME(section_page_t *page, FIELD_TYPE FIELD_NAME) \
    {                                                                                        \
        page->FIELD_NAME = FIELD_NAME;                                                       \
        FWRITE_OR_FAIL(&page->FIELD_NAME, sizeof(FIELD_TYPE), page->filp);                   \
        return OK;                                                                           \
    }

/**
 * Write inner information into file
 */
PerformStatus section_page_sync(section_page_t *page)
{
    long prev_ptr = ftell(page->filp);

    FWRITE_OR_FAIL(&page->free_space, sizeof(page->free_space), page->filp);
    FWRITE_OR_FAIL(&page->next, sizeof(page->next), page->filp);
    FWRITE_OR_FAIL(&page->last_item_ptr, sizeof(page->last_item_ptr), page->filp);
    FWRITE_OR_FAIL(&page->first_record_ptr, sizeof(page->first_record_ptr), page->filp);

    FSEEK_OR_FAIL(page->filp, prev_ptr);

    return OK;
}

sectoff_t section_page_size(section_page_t *page)
{
    return sizeof(page->free_space) + sizeof(page->next) + sizeof(page->first_record_ptr) + sizeof(page->last_item_ptr);
}

section_page_t *section_page_new()
{
    return my_malloc(section_page_t);
}

void section_page_ctor(section_page_t *page, FILE *filp)
{
    assert(filp != NULL);

    page->free_space = SECTION_SIZE - section_page_size(page);
    page->next = 0; // If we have 0 then we don't have next section
    page->last_item_ptr = ftell(filp) + section_page_size(page);
    page->first_record_ptr = ftell(filp) + SECTION_SIZE - 1;
    page->filp = filp;

    section_page_sync(page);
}
void section_page_dtor(section_page_t *page)
{
    free(page);
}

PerformStatus section_page_shift_last_item_ptr(section_page_t *page, sectoff_t shift)
{
    page->free_space -= shift;
    page->last_item_ptr += shift;

    section_page_sync(page);

    return OK;
}

PerformStatus section_page_shift_first_record_ptr(section_page_t *page, sectoff_t shift)
{
    page->free_space -= shift;
    page->first_record_ptr += shift;

    section_page_sync(page);

    return OK;
}

SECTION_PAGE_FIELD_SETTER_DEFINE(sectoff_t, free_space);
SECTION_PAGE_FIELD_SETTER_DEFINE(sectoff_t, next);
SECTION_PAGE_FIELD_SETTER_DEFINE(sectoff_t, last_item_ptr);
SECTION_PAGE_FIELD_SETTER_DEFINE(sectoff_t, first_record_ptr);