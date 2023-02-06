#pragma once

#include "stddef.h"

#include "xml/xml_node.h"

#include "utils.h"

typedef struct
{
    string_t *name;
    struct xml_node_t *root; // Nullable
} xml_document_t;

xml_document_t *xml_document_new();

void xml_document_ctor(xml_document_t *);
void xml_document_dtor(xml_document_t *);

int xml_document_get_name_size(xml_document_t *);