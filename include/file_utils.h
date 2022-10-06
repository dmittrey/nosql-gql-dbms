#pragma once 

#include <stdio.h>

#include "utils.h"

#define READ_WRITE "r+"
#define READ_WRITE_NEW "w+"

typedef const char* file_path_t;

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

#define IS_VALID_TRANSACTION(transaction_result) transaction_result.status

static struct TransactionResult performFileOpen(file_path_t file_path, const char* op_flag) {
    FILE *open_file = fopen(file_path, op_flag);

    return (open_file) ? TRANSACTION_OK(open_file) : TRANSACTION_FAILED();
}

struct TransactionResult openFile(file_path_t file_path) {
    return performFileOpen(file_path, READ_WRITE);
}

struct TransactionResult createOrBlankFile(file_path_t file_path) {
    return performFileOpen(file_path, READ_WRITE_NEW);
}