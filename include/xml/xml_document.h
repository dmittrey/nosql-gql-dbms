#pragma once

#include "stddef.h"

#include "xml/xml_node.h"

#include "utils.h"

struct xml_document_t;

struct xml_document_t *xml_document_new();

void xml_document_ctor(struct xml_document_t *document, string_t *name, struct xml_node_t *root);
void xml_document_dtor(struct xml_document_t *);