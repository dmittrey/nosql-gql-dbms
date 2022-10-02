#pragma once

/*
Structures and functions to handle our table schema(raw table with columns)
*/

#include "table_types.h"

typedef const char *const column_name_t;

enum TableStoreTableSchemaOperations
{
    RESTRICT = -1,
    EXPAND = 1
};

struct TableStoreTableSchemaColumnInfo
{
    column_name_t name;
    TableStoreTableSchemaColumnTypes type;
};

struct TableStoreTableSchemaInfo
{
    TableStoreTableSchemaColumnInfo columns[];
};

void tableStoreExpandTableSchema(TableStoreTableSchemaInfo table_schema, column_name_t column_name,
                                 TableStoreTableSchemaColumnTypes type, TableStoreTableSchemaOperations op);