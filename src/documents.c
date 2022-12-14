#include "documents.h"
#include "page_p.h"

#include "xml.h"

typedef struct
{
    page_t page;
    document_t *f_document;
} documents_t;
