#include <assert.h>

#include "../include/table_file.h"

static void checkCreateSections(struct TransactionResult file) {
    enum PerformStatus docCreateStatus = createDocumentsSection(file.file, 0, NULL);

    assert(docCreateStatus != FAILED);

    fseek(file.file, 0, SEEK_SET);
    uint64_t *doc_buffer = (uint64_t *)malloc(sizeof(uint64_t) * 4);

    fread(doc_buffer, sizeof(uint64_t), 4, file.file);

    assert(doc_buffer[0] != 0);                                          // Length not empty
    assert(doc_buffer[1] == NULL || doc_buffer[1] > DOCUMENTS_SECTION_SIZE); // Next section not in own
    assert(doc_buffer[2] < DOCUMENTS_SECTION_SIZE);                      // Last item on section
    assert(doc_buffer[3] <= DOCUMENTS_SECTION_SIZE);                      // First record in section
    assert(doc_buffer[2] < doc_buffer[3]);                                   // Last item placed before first buffer

    // TODO dangerous +1
    enum PerformStatus extCreateStatus = createExtentSection(file.file, DOCUMENTS_SECTION_SIZE + 1, NULL);

    assert(extCreateStatus != FAILED);

    fseek(file.file, DOCUMENTS_SECTION_SIZE + 1, SEEK_SET);
    uint64_t *ext_buffer = (uint64_t *)malloc(sizeof(uint64_t) * 4);

    fread(ext_buffer, sizeof(uint64_t), 4, file.file);

    assert(ext_buffer[0] != 0);                                          // Length not empty
    assert(ext_buffer[1] == NULL || ext_buffer[1] > DOCUMENTS_SECTION_SIZE + EXTENT_SECTION_SIZE); // Next section not in own
    assert(ext_buffer[2] > DOCUMENTS_SECTION_SIZE && ext_buffer[2] < DOCUMENTS_SECTION_SIZE + EXTENT_SECTION_SIZE); // Last item on section
    assert(ext_buffer[3] > DOCUMENTS_SECTION_SIZE && ext_buffer[3] <= DOCUMENTS_SECTION_SIZE + EXTENT_SECTION_SIZE); // First record in section
    assert(ext_buffer[2] < ext_buffer[3]);                                   // Last item placed before first buffer
}

int main(int argc, char **argv)
{
    struct TransactionResult file = openOrCreateTableStoreFile("/Users/dmitry/Desktop/low-level-programming/test.txt");

    checkCreateSections(file);

    closeTableStoreFile(file.file);
}
