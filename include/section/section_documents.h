#pragma once

#include "section/section_page_p.h"

#include "xml/xml_document.h"

#include "utils.h"

struct section_documents_t;

struct section_documents_t *page_new();

void page_ctor(struct section_documents_t *, FILE *filep);
void page_dtor(struct section_documents_t *);

enum PerformStatus section_documents_write(struct section_documents_t *, struct xml_document_t *);
enum PerformStatus section_documents_read(struct section_documents_t *, struct xml_document_t *);