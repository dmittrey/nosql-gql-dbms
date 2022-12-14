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
struct page_t;

struct page_t *page_new();

void page_ctor(struct page_t *, FILE *filep);
void page_dtor(struct page_t *);

PerformStatus page_write(struct page_t *, sectoff_t, node_t *);
PerformStatus page_read(struct page_t *, sectoff_t, node_t *);