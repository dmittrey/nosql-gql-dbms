#include "table_file.h"

//TODO Session logs? (f.e. Not opened files etc.)

static struct TransactionResult performFileOpen(file_path_t file_path, const char* op_flag);

static struct TransactionResult openFile(file_path_t file_path);

static struct TransactionResult createOrBlankFile(file_path_t file_path);










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
    fclose((FILE*)file);
    INFO("File closed!");
}

enum PerformStatus createDocumentsSection(
    table_file_t *file,
    struct Documents* prev_section // Can be null if section is first
) {

}

enum PerformStatus createExtentSection(
    table_file_t *file,
    struct Extent* prev_section // Can be null if section is first
) {
    
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