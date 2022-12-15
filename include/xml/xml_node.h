#pragma once

#include "stddef.h"

#include "xml/xml_document.h"

#include "utils.h"

#define XML_NODE_GET_DECLARE(FIELD_TYPE, FIELD) FIELD_TYPE xml_node_get_##FIELD(struct xml_node_t *node)

#define XML_NODE_SET_DECLARE(FIELD_TYPE, FIELD) void xml_node_set_##FIELD(struct xml_node_t *node, FIELD_TYPE new_##FIELD)

struct xml_node_t;

enum type_of_node_t
{
    START_TAG = 0, // <address>
    END_TAG,       // </address>
    TEXT,
    EMPTY_TAG // <hr />
};

struct attribute_t
{
    string_t name;
    string_t value;
};

struct xml_node_t *xml_node_new();

void xml_node_ctor(struct xml_node_t *, struct xml_document_t *, struct xml_node_t *, string_t *,
                   enum type_of_node_t *, struct attribute_t **, struct xml_node_t **, string_t *);
void xml_node_dtor(struct xml_node_t *);

/* Getters */
XML_NODE_GET_DECLARE(struct xml_document_t *, document);
XML_NODE_GET_DECLARE(struct xml_node_t *, parent);
XML_NODE_GET_DECLARE(string_t *, name);
XML_NODE_GET_DECLARE(enum type_of_node_t *, type);
XML_NODE_GET_DECLARE(struct attribute_t **, attributes);
XML_NODE_GET_DECLARE(struct xml_node_t **, children);
XML_NODE_GET_DECLARE(string_t *, text);

/* Setters */
XML_NODE_SET_DECLARE(struct xml_document_t *, document);
XML_NODE_SET_DECLARE(struct xml_node_t *, parent);
XML_NODE_SET_DECLARE(string_t *, name);
XML_NODE_SET_DECLARE(enum type_of_node_t *, type);
XML_NODE_SET_DECLARE(struct attribute_t **, attributes);
XML_NODE_SET_DECLARE(struct xml_node_t **, children);
XML_NODE_SET_DECLARE(string_t *, text);