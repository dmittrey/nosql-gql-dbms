#include "utils.h"
#include "table.h"

#include "section/section_page.h"
#include "section/section_documents.h"

// #define SAVE_BUF_PTR(FILP, CODE)           \
//     fileoff_t start_fileoff = ftell(FILP); \
//     CODE;                                  \
//     SEEK_OR_FAIL(FILP, start_fileoff);

// /* Declarations */
// static PerformStatus section_documents_write(struct section_documents_t *, sectoff_t, struct xml_document_t *);

// section_documents_t *section_documents_new()
// {
//     return my_malloc(section_documents_t);
// }

// void section_documents_ctor(struct section_documents_t *section, FILE *filp, fileoff_t fileoff)
// {
//     section_page_ctor((section_page_t *)section, filp, fileoff);
// }
// void section_documents_dtor(section_documents_t *section)
// {
//     section_page_dtor((section_page_t *)section);
// }

// PerformStatus section_documents_sync(section_documents_t *section)
// {
//     return section_page_sync((section_page_t *)section);
// }

// PerformStatus section_documents_insert(section_documents_t *section, struct xml_document_t *xml_document, fileoff_t f_node_addr)
// {
//     section_page_t *section_page = (section_page_t *)section;
//     sectoff_t last_item_ptr = section_page->last_item_ptr;
//     sectoff_t first_record_ptr = section_page_get_last_item_ptr(section_page);

//     fileoff_t start_fileoff = ftell(section_page->filp);

//     section_documents_write_name(section, first_record_ptr, xml_document);

//     int xml_document_name_count = xml_document_get_name_count(xml_document);
//     SEEK_OR_FAIL(section_page->filp, section_page->fileoff + last_item_ptr);
//     FWRITE_OR_FAIL(&xml_document_name_count, sizeof(uint64_t), section_page->filp);

//     last_item_ptr += sizeof(uint64_t);
//     int xml_document_name_size = xml_document_get_name_size(section);
//     sectoff_t xml_document_name_addr = first_record_ptr - xml_document_name_size + 1;
//     SEEK_OR_FAIL(section_page->filp, section_page->fileoff + last_item_ptr);
//     FWRITE_OR_FAIL(&xml_document_name_addr, sizeof(uint64_t), section_page->filp);

//     last_item_ptr += sizeof(uint64_t);
//     SEEK_OR_FAIL(section_page->filp, section_page->fileoff + last_item_ptr);
//     FWRITE_OR_FAIL(&f_node_addr, sizeof(uint64_t), section_page->filp);

//     section_page_set_free_space(section_page->filp, section_page->free_space - 3 * sizeof(uint64_t) - xml_document_name_size);
//     section_page_set_last_item_ptr(section_page->filp, last_item_ptr + sizeof(uint64_t));
//     section_page_set_first_record_ptr(section_page->filp, xml_document_name_addr - 1);

//     SEEK_OR_FAIL(section_page->filp, start_fileoff);
// }

// /**
//  * Сначала страницы вставляется item long который указывает на tuple который начинается с конца
//  */
// // static PerformStatus section_documents_write_name(section_documents_t *section, sectoff_t name_end_addr, struct xml_document_t *xml_document)
// // {
// //     section_page_t *section_page = (section_page_t *)section;

// //     SAVE_BUF_PTR(section_page->filp,
// //                  int xml_document_name_size = xml_document_get_name_size(xml_document);
// //                  SEEK_OR_FAIL(section_page->filp, section_page->fileoff + name_end_addr - xml_document_name_size + 1); // TODO +1?
// //                  FWRITE_OR_FAIL(xml_document_get_name_val(xml_document), xml_document_name_size, section_page->filp));

// //     return OK;
// // }
// // static PerformStatus section_documents_read_name(section_documents_t *section, sectoff_t name_start_addr, sectoff_t name_len, string_t *name)
// // {
// //     section_page_t *section_page = (section_page_t *)section;

// //     SAVE_BUF_PTR(section_page->filp,
// //                  char *name_val = my_malloc_array(char, name_len);
// //                  SEEK_OR_FAIL(section_page->filp, section_page->fileoff + name_start_addr);
// //                  FREAD_OR_FAIL(name_val, sizeof(char) * name_len, section_page->filp);
// //                  name->val = name_val;
// //                  name->count = name_len);

// //     return OK;
// // }