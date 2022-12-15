#include "utils.h"

#define XML_NODE_GET_DEFINE(FIELD_TYPE, FIELD)        \
    FIELD_TYPE xml_node_get_##FIELD(xml_node_t *node) \
    {                                                 \
        return node->FIELD;                           \
    }

#define XML_NODE_SET_DEFINE(FIELD_TYPE, FIELD)                          \
    void xml_node_set_##FIELD(xml_node_t *node, FIELD_TYPE new_##FIELD) \
    {                                                                   \
        node->FIELD = new_##FIELD;                                      \
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
                   struct type_of_node_t *type, struct attribute_t **attributes, xml_node_t **children, string_t *text)
{
    my_node->document = document;
    my_node->parent = parent;
    my_node->name = name;
    my_node->type = type;
    my_node->attributes = attributes;
    my_node->text = text;
}
void xml_node_dtor(xml_node_t *my_node)
{
    free(my_node->document);
    free(my_node->parent);
    free(my_node->name);
    free(my_node->type);
    free(my_node->attributes);
    free(my_node->children);
    free(my_node->text);
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