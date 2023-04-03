#pragma once

#include "utils/string.h"

#include "query/query.h"

#include "json/json.h"

typedef enum CommandType
{
    INSERT = 0,
    READ,
    UPDATE,
    DELETE
} CommandType;

typedef struct Command
{
    CommandType type;
    String *type_name;
    Query *query;
    Json *json;
} Command;