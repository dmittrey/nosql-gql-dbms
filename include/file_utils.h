#include "table_file.h"
#include "stdio.h"

#define READ_WRITE "r+"
#define READ_WRITE_NEW "w+"

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