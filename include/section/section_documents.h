#pragma once

#include "section/section_page_p.h"

#include "xml/xml_document.h"

#include "utils.h"

struct section_documents_t;

struct section_documents_t *section_documents_new();

void section_documents_ctor(struct section_documents_t *, FILE *, fileoff_t);
void section_documents_dtor(struct section_documents_t *);

enum PerformStatus section_documents_sync(struct section_documents_t *);

enum PerformStatus section_documents_insert(struct section_documents_t *, struct xml_document_t *);
enum PerformStatus section_documents_read(struct section_documents_t *, struct xml_document_t *);
enum PerformStatus section_documents_update(struct section_documents_t *, struct xml_document_t *);
enum PerformStatus section_documents_delete(struct section_documents_t *, struct xml_document_t *);