#pragma once

/*
Structures and functions to handle our table schema(raw table with columns)
*/

#include "table_types.h"

typedef const char *const column_name_t;

typedef enum TableStoreTableSchemaOperations
{
    RESTRICT = -1,
    EXPAND = 1
} TableStoreTableSchemaOperations;

typedef struct TableStoreTableSchemaColumnInfo
{
    column_name_t name;
    TableStoreTableSchemaColumnTypes type;
} TableStoreTableSchemaColumnInfo;

//Норм ли решение?
typedef struct TableStoreTableSchemaInfo
{
    TableStoreTableSchemaColumnInfo *columns;
} TableStoreTableSchemaInfo;

void tableStoreExpandTableSchema(TableStoreTableSchemaInfo table_schema, column_name_t column_name,
                                 TableStoreTableSchemaColumnTypes type, TableStoreTableSchemaOperations op);