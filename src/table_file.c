#include "table_file.h"
#include "file_utils.h"
#include "stdio.h"

#define SUCCESSFUL 0
#define NOT_OPENED 1

//TODO Session logs? (f.e. Not opened files etc.)

struct TransactionResult openOrCreateTableStoreFile(file_path_t file_path) {
    struct TransactionResult t = openFile(file_path);
    
    // If we opened file => exist and we can wrap into transaction
    if (!IS_VALID_TRANSACTION(t)) {
        t = createOrBlankFile(file_path);
        
        if (!IS_VALID_TRANSACTION(t)) {
            return TRANSACTION_FAILED();
        }
    }

    return t;
};

struct TransactionResult closeTableStoreFile(table_file_t *file) {
    fclose(file);
};