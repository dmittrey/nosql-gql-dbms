#include "utils.h"

#include "xml/xml_document.h"

xml_document_t *xml_document_new()
{
    return my_malloc(xml_document_t);
}

void xml_document_ctor(xml_document_t *document)
{
}
void xml_document_dtor(xml_document_t *document)
{
    free(document);
}