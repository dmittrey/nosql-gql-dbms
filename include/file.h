#pragma once

/*
Structures and functions to handle our file with tables
*/

#include <stdio.h>
#include <stdbool.h>

typedef char *file_path_t;

// Structure to handle database file
typedef struct TableStoreFileHandle
{
    FILE* file;
    bool isOpen;
} TableStoreFileHandle;

TableStoreFileHandle tableStoreFileOpenOrCreate(file_path_t file_path);

TableStoreFileHandle tableStoreFileClose(TableStoreFileHandle *file_handler);
