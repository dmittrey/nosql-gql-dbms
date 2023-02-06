#pragma once

#include "stddef.h"

#include "xml/xml_document.h"

#include "utils.h"

typedef enum
{
    START_TAG = 0, // <address>
    END_TAG,       // </address>
    TEXT,
    EMPTY_TAG // <hr />
} type_of_node_t;

typedef struct
{
    string_t name;
    string_t value;
} attribute_t;

typedef struct
{
    struct xml_document_t *document;
    struct xml_node_t *parent;
    string_t *name;
    struct type_of_node_t *type;
    struct attribute_t **attributes;
    struct xml_node_t **children;
    string_t *text;
} xml_node_t;

xml_node_t *xml_node_new();

void xml_node_ctor(xml_node_t *);
void xml_node_dtor(xml_node_t *);