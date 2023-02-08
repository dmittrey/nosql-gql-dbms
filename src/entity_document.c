#include "utils.h"
#include "table.h"

#include "entity/entity_document.h"

entity_document_t *entity_document_new() 
{
    return my_malloc(entity_document_t);
}