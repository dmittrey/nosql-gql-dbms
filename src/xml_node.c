#include "utils.h"

#define XML_NODE_GET_DEFINE(FIELD_TYPE, FIELD)        \
    FIELD_TYPE xml_node_get_##FIELD(xml_node_t *node) \
    {                                                 \
        return node->FIELD;                           \
    }

#define XML_NODE_SET_DEFINE(FIELD_TYPE, FIELD)                          \
    bool xml_node_set_##FIELD(xml_node_t *node, FIELD_TYPE new_##FIELD) \
    {                                                                   \
        node->FIELD = new_##FIELD;                                      \
        return false;                                                   \
    }

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

xml_node_t *xml_node_new()
{
    return my_malloc(xml_node_t);
}

void xml_node_ctor(xml_node_t *my_node, struct xml_document_t *document, xml_node_t *parent, string_t *name,
                   struct type_of_node_t *type, struct attribute_t **attributes, xml_node_t **children, string_t *text) {}
void xml_node_dtor(xml_node_t *node) {}

size_t xml_node_get_size(xml_node_t *node)
{
    return 5;
}

XML_NODE_GET_DEFINE(struct xml_document_t *, document);
XML_NODE_GET_DEFINE(struct xml_node_t *, parent);
XML_NODE_GET_DEFINE(string_t *, name);
XML_NODE_GET_DEFINE(struct type_of_node_t *, type);
XML_NODE_GET_DEFINE(struct attribute_t **, attributes);
XML_NODE_GET_DEFINE(struct xml_node_t **, children);
XML_NODE_GET_DEFINE(string_t *, text);

XML_NODE_SET_DEFINE(struct xml_document_t *, document);
XML_NODE_SET_DEFINE(struct xml_node_t *, parent);
XML_NODE_SET_DEFINE(string_t *, name);
XML_NODE_SET_DEFINE(struct type_of_node_t *, type);
XML_NODE_SET_DEFINE(struct attribute_t **, attributes);
XML_NODE_SET_DEFINE(struct xml_node_t **, children);
XML_NODE_SET_DEFINE(string_t *, text);