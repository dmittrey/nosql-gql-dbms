#pragma once

#include "table.h"

#include "xml/xml_document.h"

typedef struct
{
    uint64_t name_length;
    sectoff_t name_ptr;
    fileoff_t first_node_addr;
} entity_document_t;

entity_document_t *entity_document_new();