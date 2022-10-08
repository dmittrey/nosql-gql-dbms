#include "table_file.h"

int main (int argc, char** argv) {
    struct TransactionResult file = openOrCreateTableStoreFile("/Users/dmitry/Desktop/low-level-programming/test.txt");

    closeTableStoreFile(file.file);
}
