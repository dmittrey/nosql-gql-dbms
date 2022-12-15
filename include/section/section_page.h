#pragma once

#include "utils.h"

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

void section_page_ctor(struct section_page_t *, FILE *filep);
void section_page_dtor(struct section_page_t *);

enum PerformStatus section_page_write(struct section_page_t *, sectoff_t, xml_node_t *);
enum PerformStatus section_page_read(struct section_page_t *, sectoff_t, xml_node_t *);