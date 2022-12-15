#pragma once

#include "section/section_page_p.h"

#include "xml/xml_document.h"

#include "utils.h"

struct section_extent_t;

struct section_extent_t *page_new();

void page_ctor(struct section_extent_t *, FILE *filep);
void page_dtor(struct section_extent_t *);

enum PerformStatus section_extent_write(struct section_extent_t *, struct xml_node_t *);
enum PerformStatus section_extent_read(struct section_extent_t *, struct xml_node_t *);