#include "utils.h"

#include "xml/xml_node.h"

xml_node_t *xml_node_new()
{
    return my_malloc(xml_node_t);
}

void xml_node_ctor(xml_node_t *my_node)
{
    
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
    free(my_node);
}