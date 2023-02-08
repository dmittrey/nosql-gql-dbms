#include "utils.h"
#include "table.h"

#include "entity/entity_document.h"

#include "section/section_page.h"
#include "section/section_documents.h"

// /* Declarations */
// static PerformStatus section_documents_write(struct section_documents_t *, sectoff_t, struct xml_document_t *);

section_documents_t *section_documents_new()
{
    return my_malloc(section_documents_t);
}

void section_documents_ctor(section_documents_t *section, fileoff_t offset, FILE *filp)
{
    section_page_ctor((section_page_t *)section, offset, filp);
}
void section_documents_dtor(section_documents_t *section)
{
    section_page_dtor((section_page_t *)section);
}

PerformStatus section_documents_sync(section_documents_t *section)
{
    return section_page_sync((section_page_t *)section);
}

PerformStatus section_documents_write(section_documents_t *section, xml_document_t *xml_document, entity_document_t *entity_document)
{
    if (section->header.free_space <= ITEM_SIZE + xml_document_get_name_size(xml_document))
    {
        long prev_ptr = ftell(section->header.filp);

        // Write name in first record
        section_page_shift_first_record_ptr((section_page_t *)section, -1 * xml_document_get_name_size(xml_document));
        FSEEK_OR_FAIL(section->header.filp, section->header.first_record_ptr);
        FWRITE_OR_FAIL(xml_document->name->val, xml_document_get_name_size(xml_document), section->header.filp);

        // Write name_length, name_ptr, first_node_addr in first free record
        FSEEK_OR_FAIL(section->header.filp, section->header.last_item_ptr);
        FWRITE_OR_FAIL(&xml_document->name->count, sizeof(uint32_t), section->header.filp);
        FWRITE_OR_FAIL(&section->header.first_record_ptr, sizeof(uint32_t), section->header.filp);
        // TODO Add first node addr
        section_page_shift_last_item_ptr((section_page_t *)section, ITEM_SIZE);

        FSEEK_OR_FAIL(section->header.filp, prev_ptr);

        entity_document->name_length = xml_document->name->count;
        entity_document->name_ptr = section->header.first_record_ptr;
        // entity_document->first_node_addr TODO

        return OK;
    }
    else
    {
        entity_document = NULL;
        return FAILED;
    }
}

/*
- Пройтись по ITEM
- Проверить соответствует ли имя(сначала длина, потом имя)
*/
PerformStatus section_documents_read(section_documents_t *section, xml_document_t *xml_document, entity_document_t *entity_document)
{
    for (sectoff_t item_ptr = section->header)
}

PerformStatus section_documents_update(section_documents_t *section, xml_document_t *xml_document, entity_document_t *entity_document)
{
}

PerformStatus section_documents_delete(section_documents_t *section, xml_document_t *xml_document, entity_document_t *entity_document)
{
}