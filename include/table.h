#pragma once

/*
Structures and functions to handle datatables
*/

#include "file.h"
#include "schema.h"

typedef const char *const table_name_t;
typedef u_int64_t pr_key_t;

typedef struct Namespace {
    size_t count;
    struct Extent *first_table_extent[];
} Namespace;

typedef struct TableStoreTableHandle
{
    TableStoreFileHandle *file;
    table_name_t name;
    TableStoreTableSchemaInfo *tables_schema;
    Namespace *namespaces;
} TableStoreTableHandle;

typedef struct TableStoreRecordData {
    size_t count;
    TableStoreRecord *params[];
} TableStoreRecordData;

TableStoreTableHandle tableStoreFileCreateTable(TableStoreFileHandle* file,
                                                table_name_t table_name, TableStoreTableSchemaInfo table_schema);

TableStoreTableHandle tableStoreFileOpenTable(TableStoreFileHandle* file, table_name_t table_name);

void tableStoreFileCloseTable(TableStoreTableHandle *file_handler);

void tableStoreFileSeekTable(TableStoreTableHandle *file_handler, size_t offset, int seek_option);

void tableStoreAddRecord(TableStoreFileHandle *file_handler, TableStoreRecordData *record);

TableStoreRecordData* tableStoreFilePrepareRecordDataStructure(TableStoreTableHandle *table_handler);

void tableStoreFileReadRecord(TableStoreTableHandle *table_handler, TableStoreRecordData *record);

void tableStoreFileCleanupRecordDataStructure(TableStoreRecordData *record);

