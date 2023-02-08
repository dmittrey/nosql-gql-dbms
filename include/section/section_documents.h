#pragma once

#include "utils.h"
#include "table.h"

#include "xml/xml_document.h"

#include "section/section_page.h"

#define ITEM_SIZE 3 * sizeof(uint64_t)

typedef struct
{
    section_page_t header;
} section_documents_t;

section_documents_t *section_documents_new();

void section_documents_ctor(section_documents_t *, fileoff_t, FILE *);
void section_documents_dtor(section_documents_t *);

PerformStatus section_documents_sync(section_documents_t *);

PerformStatus section_documents_write(section_documents_t *, xml_document_t *, entity_document_t *);
PerformStatus section_documents_read(section_documents_t *, xml_document_t *, entity_document_t *);
PerformStatus section_documents_update(section_documents_t *, xml_document_t *, entity_document_t *);
PerformStatus section_documents_delete(section_documents_t *, xml_document_t *, entity_document_t *);