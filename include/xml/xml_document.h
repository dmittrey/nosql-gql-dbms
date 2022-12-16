#pragma once

#include "stddef.h"

#include "xml/xml_node.h"

#include "utils.h"

struct xml_document_t;

struct xml_document_t *xml_document_new();

void xml_document_ctor(struct xml_document_t *, string_t *, struct xml_document_t *);
void xml_document_dtor(struct xml_document_t *);

int xml_document_get_name_count(struct xml_document_t *);
char *xml_document_get_name_val(struct xml_document_t *);

int xml_document_get_name_size(struct xml_document_t *);