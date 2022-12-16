#include "utils.h"

typedef struct
{
    string_t *name;
    struct xml_node_t *root; // Nullable
} xml_document_t;

xml_document_t *xml_document_new()
{
    return my_malloc(xml_document_t);
}

void xml_document_ctor(xml_document_t *document, string_t *name, struct xml_document_t *root)
{
    document->name = name;
    document->root = root;
}
void xml_document_dtor(xml_document_t *document)
{
    free(document);
}

int xml_document_get_name_count(xml_document_t *document)
{
    return document->name.count;
}
char *xml_document_get_name_val(xml_document_t *document)
{
    return document->name.val;
}
int xml_document_get_name_size(xml_document_t *document)
{
    return xml_document_get_name_count(document) * sizeof(char);
}