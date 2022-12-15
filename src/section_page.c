#include <stdio.h>

#include "utils.h"
#include "table.h"

#define FWRITE_OR_FAIL(DATA_PTR, DATA_SIZE, FILP)  \
    if (fwrite(DATA_PTR, DATA_SIZE, 1, FILP) != 1) \
    return FAILED

#define SECTION_PAGE_FIELD_GETTER_DEFINE(FIELD_TYPE, FIELD_NAME)   \
    FIELD_TYPE section_page_get_##FIELD_NAME(section_page_t *page) \
    {                                                              \
        return page->FIELD_NAME;                                   \
    }

#define SECTION_PAGE_FIELD_SETTER_DEFINE(FIELD_TYPE, FIELD_NAME)                                  \
    enum PerformStatus section_page_set_##FIELD_NAME(section_page_t *page, FIELD_TYPE FIELD_NAME) \
    {                                                                                             \
        page->FIELD_NAME = FIELD_NAME;                                                            \
        FWRITE_OR_FAIL(&page->FIELD_NAME, sizeof(FIELD_TYPE), page->filp);                        \
        return OK;                                                                                \
    }

typedef struct
{
    sectoff_t free_space;
    sectoff_t next;             // Nullable
    sectoff_t last_item_ptr;    // Pointer to first free cell(after items)
    sectoff_t first_record_ptr; // Pointer to last free cell(before records)
    fileoff_t fileoff;
    FILE *filp;
} section_page_t;

section_page_t *section_page_new()
{
    return my_malloc(section_page_t);
}

void section_page_ctor(section_page_t *page, FILE *filp, fileoff_t fileoff)
{
    page->free_space = SECTION_SIZE - sizeof(section_page_t);
    page->next = 0; // If we have 0 then we don't have next section
    page->last_item_ptr = sizeof(section_page_t);
    page->first_record_ptr = SECTION_SIZE - 1;
    page->fileoff = fileoff;
    page->filp = filp;
}
void section_page_dtor(section_page_t *page)
{
    free(page);
}

/**
 * Write inner information into file
 */
enum PerformStatus section_page_sync(section_page_t *page)
{
    FWRITE_OR_FAIL(&page->free_space, sizeof(page->free_space), page->filp);
    FWRITE_OR_FAIL(&page->next, sizeof(page->next), page->filp);
    FWRITE_OR_FAIL(&page->last_item_ptr, sizeof(page->last_item_ptr), page->filp);
    FWRITE_OR_FAIL(&page->first_record_ptr, sizeof(page->first_record_ptr), page->filp);

    return OK;
}

SECTION_PAGE_FIELD_GETTER_DEFINE(sectoff_t, free_space);
SECTION_PAGE_FIELD_GETTER_DEFINE(sectoff_t, next);
SECTION_PAGE_FIELD_GETTER_DEFINE(sectoff_t, last_item_ptr);
SECTION_PAGE_FIELD_GETTER_DEFINE(sectoff_t, first_record_ptr);

SECTION_PAGE_FIELD_SETTER_DEFINE(sectoff_t, free_space);
SECTION_PAGE_FIELD_SETTER_DEFINE(sectoff_t, next);
SECTION_PAGE_FIELD_SETTER_DEFINE(sectoff_t, last_item_ptr);
SECTION_PAGE_FIELD_SETTER_DEFINE(sectoff_t, first_record_ptr);