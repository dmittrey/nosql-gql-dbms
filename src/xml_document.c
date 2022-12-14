#include "utils.h"

typedef struct
{
    string_t *name;
    struct xml_node_t *root; // Nullable
} xml_document_t;

xml_document_t *xml_document_new() {}

void xml_document_ctor(xml_document_t *document, string_t *name, struct xml_node_t *root) {}
void xml_document_dtor(xml_document_t *document) {}