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

/* Section manipulation */

/*
Creates document section at the place, where is buffer ptr
*/
enum PerformStatus createDocumentsSection(
    table_file_t *table_file,
    fileoff_t location,
    struct Documents* prev_section // Can be null if section is first
) {
    FILE* file = to_FILE(table_file);

    SEEK_OR_RETURN_FAIL(file, location);

    // Set prev next ptr to next
    if (prev_section != NULL) {
        prev_section->header->next = location;
    }
    
    // Alloc and write header
    struct Header *header = my_malloc(struct Header);
    header->length = DOCUMENTS_SECTION_SIZE;
    header->next = NULL_NEXT;
    //TODO need +1??
    header->last_item_ptr = location + sizeof(header->length) + sizeof(header->next) + 1;
    //TODO need -1??
    header->first_record_ptr = location + DOCUMENTS_SECTION_SIZE - 1;

    return createHeader(file, header, location);
}

enum PerformStatus createExtentSection(
    table_file_t *table_file,
    fileoff_t location,
    struct Extent* prev_section // Can be null if section is first
) {
    FILE* file = to_FILE(table_file);

    SEEK_OR_RETURN_FAIL(file, location);

    // Set prev next ptr to next
    if (prev_section != NULL) {
        prev_section->header->next = location;
    }
    
    // Alloc and write header
    struct Header *header = my_malloc(struct Header);
    header->length = EXTENT_SECTION_SIZE;
    header->next = NULL_NEXT;
    //TODO need +1??
    header->last_item_ptr = location + sizeof(header->length) + sizeof(header->next) + 1;
    //TODO need -1??
    header->first_record_ptr = location + DOCUMENTS_SECTION_SIZE - 1;

    return createHeader(file, header, location);
}

/* Data manipulation */

/*
Firstly, we check free space and decide to put it in this section or create new

Secondly, we going to end of items and put information about document. 
Document's name we put at the end of section before older names.
*/
enum PerformStatus insertDocument(table_file_t *file, struct Document *document) {

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
    size_t wroten = 0;
    
    wroten = fwrite(&header->length, sizeof(header->length), 1, file);
    if (wroten != 1) return FAILED; // fwrite returns successfully written objects

    wroten = fwrite(&header->next, sizeof(header->next), 1, file);
    if (wroten != 1) return FAILED;

    wroten = fwrite(&header->last_item_ptr, sizeof(header->last_item_ptr), 1, file);
    if (wroten != 1) return FAILED;

    wroten = fwrite(&header->first_record_ptr, sizeof(header->first_record_ptr), 1, file);
    if (wroten != 1) return FAILED;

    return OK;
}