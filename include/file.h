#pragma once

/*
Structures and functions to handle our file with tables
*/

#include <stdio.h>

typedef char *file_path_t;

// Structure to handle database file
struct TableStoreFileHandle
{
    FILE* file;
    bool isOpen;
};

TableStoreFileHandle tableStoreFileOpenOrCreate(file_path_t file_path);

TableStoreFileHandle tableStoreFileClose(TableStoreFileHandle *file_handler);
