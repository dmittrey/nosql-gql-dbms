#pragma once 

#include <stdio.h>

#include "utils.h"

#define READ_WRITE "rb+"
#define READ_WRITE_NEW "wb+"

typedef const char* file_path_t;

#define to_FILE(file) (FILE*) file

#define SEEK_OR_RETURN_FAIL(file, offset) if (fseek(file, offset, SEEK_SET) != 0) { \
        return FAILED; \
    };

static size_t writeUInt64ToFile(FILE *file, uint64_t val) {
    return fwrite(&val, sizeof(val), 1, file);
}

#define WRITE_UINT64_OR_RETURN_FAIL(file, value) size_t wroten = writeUInt64ToFile(file, value); \
    if (wroten != sizeof(value)) { \
        return FAILED; \
    }

// Own program abstraction above file 
typedef FILE * table_file_t;

// Abstraction of performing ops
struct TransactionResult {
    table_file_t file; // Nullable or ptr to start queried data
    enum PerformStatus status;
};

#define TRANSACTION_OK(file_ptr) (struct TransactionResult) { \
            .file = file_ptr, \
            .status = 0 \
        }
#define TRANSACTION_FAILED() (struct TransactionResult) { \
            .file = NULL, \
            .status = 1 \
        }

#define IS_INVALID_TRANSACTION(transaction_result) transaction_result.status 