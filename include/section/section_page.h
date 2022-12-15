#pragma once

#include "utils.h"
#include "table.h"

#define SECTION_PAGE_FIELD_GETTER_DECLARE(FIELD_TYPE, FIELD_NAME) \
    FIELD_TYPE section_page_get_##FIELD_NAME(section_page_t *page)

#define SECTION_PAGE_FIELD_SETTER_DECLARE(FIELD_TYPE, FIELD_NAME) \
    enum PerformStatus section_page_set_##FIELD_NAME(section_page_t *page, FIELD_TYPE FIELD_NAME)

/**
 * Third layer of abstractions
 *
 * ----------------------
 * | DB API             |
 * ----------------------
 * | DB FILE IO         |
 * ----------------------
 * | DB FILE SECTION IO |
 * ----------------------
 *
 * @see include/table.h
 */
struct section_page_t;

struct section_page_t *section_page_new();

void section_page_ctor(struct section_page_t *, FILE *, fileoff_t);
void section_page_dtor(struct section_page_t *);

enum PerformStatus section_page_sync(section_page_t *);

SECTION_PAGE_FIELD_GETTER_DECLARE(sectoff_t, free_space);
SECTION_PAGE_FIELD_GETTER_DECLARE(sectoff_t, next);
SECTION_PAGE_FIELD_GETTER_DECLARE(sectoff_t, last_item_ptr);
SECTION_PAGE_FIELD_GETTER_DECLARE(sectoff_t, first_record_ptr);

SECTION_PAGE_FIELD_SETTER_DECLARE(sectoff_t, free_space);
SECTION_PAGE_FIELD_SETTER_DECLARE(sectoff_t, next);
SECTION_PAGE_FIELD_SETTER_DECLARE(sectoff_t, last_item_ptr);
SECTION_PAGE_FIELD_SETTER_DECLARE(sectoff_t, first_record_ptr);