#include "../include/table_file.h"
#include <inttypes.h>

//TODO Session logs? (f.e. Not opened files etc.)

static struct TransactionResult performFileOpen(file_path_t file_path, const char* op_flag);

static struct TransactionResult openFile(file_path_t file_path);

static struct TransactionResult createOrBlankFile(file_path_t file_path);

static enum PerformStatus createHeader(FILE* file, struct Header *header, fileoff_t location);








/* File manipulation */

struct TransactionResult openOrCreateTableStoreFile(file_path_t file_path) {
    struct TransactionResult t = openFile(file_path);
    
    // If we opened file => exist and we can wrap into transaction
    if (IS_INVALID_TRANSACTION(t)) {
        t = createOrBlankFile(file_path);
        
        if (IS_INVALID_TRANSACTION(t)) {
            INFO("File cannot be opened!\n");
            return TRANSACTION_FAILED();
        }

        INFO("File created!\n");
    }

    INFO("File opened!\n");

    return t;
}

struct TransactionResult closeTableStoreFile(table_file_t *file) {
    fclose(to_FILE(file));
    INFO("File closed!");
    return TRANSACTION_FAILED();
}

/*
Creates document section at the place, where is buffer ptr
*/
enum PerformStatus createDocumentsSection(
    table_file_t *table_file,
    fileoff_t location,
    struct Documents* prev_section // Can be null if section is first
) {
    FILE* file = to_FILE(table_file);

    // Set prev next ptr to next
    if (prev_section != NULL) {
        prev_section->header->next = location;
    }
    
    // Alloc and write header
    struct Header *header = my_malloc(struct Header);
    header->length = DOCUMENTS_SECTION_SIZE;
    header->next = NULL_NEXT;

    return createHeader(file, header, location);
}

enum PerformStatus createExtentSection(
    table_file_t *table_file,
    fileoff_t location,
    struct Extent* prev_section // Can be null if section is first
) {
    FILE* file = to_FILE(table_file);

    // Set prev next ptr to next
    if (prev_section != NULL) {
        prev_section->header->next = location;
    }
    
    // Alloc and write header
    struct Header *header = my_malloc(struct Header);
    header->length = EXTENT_SECTION_SIZE;
    header->length = NULL_NEXT;

    return createHeader(file, header, location);
}













/* Static functions */

static struct TransactionResult performFileOpen (file_path_t file_path, const char* op_flag) {
    FILE *open_file = fopen(file_path, op_flag);

    return (open_file != NULL) ? TRANSACTION_OK(open_file) : TRANSACTION_FAILED();
}

static struct TransactionResult openFile(file_path_t file_path) {
    return performFileOpen(file_path, READ_WRITE);
}

static struct TransactionResult createOrBlankFile(file_path_t file_path) {
    return performFileOpen(file_path, READ_WRITE_NEW);
}

static enum PerformStatus createHeader(FILE* file, struct Header *header, fileoff_t location) {
    if (fseek(file, location, SEEK_SET) != 0) {
        return FAILED;
    };

    uint64_t * buffer = (uint64_t*) malloc(sizeof(uint64_t) * 2);

    size_t result = fread(buffer, sizeof(uint64_t), 2, file);

    printf("%" PRIu64 "\n", buffer[0]);
    printf("%" PRIu64 "\n", buffer[1]);

    fwrite(&header->length, sizeof(header->length), 1, file);
    fwrite(&header->next, sizeof(header->next), 1, file);

    return OK;
}