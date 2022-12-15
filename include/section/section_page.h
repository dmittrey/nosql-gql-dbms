#pragma once

#include "xml.h"

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

struct section_page_t *page_new();

void page_ctor(struct section_page_t *, FILE *filep);
void page_dtor(struct section_page_t *);

PerformStatus page_write(struct section_page_t *, sectoff_t, xml_node_t *);
PerformStatus page_read(struct section_page_t *, sectoff_t, xml_node_t *);