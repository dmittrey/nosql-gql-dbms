#pragma once

#include "utils/string.h"
#include "utils/list.h"

#define PATH(STR)                       \
    Path *STR##_path = my_calloc(Path); \
    STR##_path->name = STR;

typedef struct Path
{
    String *name;
    struct Path *next;
} Path;

LIST_DECLARE(Path);