#pragma once

#include "utils.h"
#include "table.h"

#include "xml/xml_document.h"

#include "section/section_page.h"

typedef struct
{
    section_page_t page;
} section_documents_t;

section_documents_t *section_documents_new();

void section_documents_ctor(section_documents_t *, FILE *, fileoff_t);
void section_documents_dtor(section_documents_t *);

PerformStatus section_documents_sync(section_documents_t *);

PerformStatus section_documents_write(section_documents_t *, struct xml_document_t *);