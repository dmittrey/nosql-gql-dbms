#pragma once 

#include <stdio.h>

#include "utils.h"

#define READ_WRITE "rb+"
#define READ_WRITE_NEW "wb+"

typedef const char* file_path_t;

#define to_FILE(file) (FILE*) file

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