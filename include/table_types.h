#pragma once

#include <stdio.h>

#define size_of_column_type(x) size_of_column_type_##x

#define size_of_column_type_TABLE_STORE_TYPE_INT32 sizeof(int32_t)
#define size_of_column_type_TABLE_STORE_TYPE_BOOL sizeof(bool)
#define size_of_column_type_TABLE_STORE_TYPE_STRING sizeof(char)
#define size_of_column_type_TABLE_STORE_TYPE_SINGLE sizeof(float)

enum TableStoreTableSchemaColumnTypes
{
    TABLE_STORE_TYPE_INT32,
    TABLE_STORE_TYPE_BOOL,
    TABLE_STORE_TYPE_STRING,
    TABLE_STORE_TYPE_SINGLE
};

struct TableStoreRecord
{
    TableStoreTableSchemaColumnTypes type;
    void *value;
};