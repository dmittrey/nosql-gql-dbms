#include "table_file.h"

int main (int argc, char** argv) {
    printf("First arg: %d", argv[0]);
    openOrCreateTableStoreFile("NAME");
}
