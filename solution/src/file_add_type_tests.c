#include <assert.h>

#include "string.h"

#include "memory/file/file.h"

#include "memory/type/type.h"

#include "memory/section/types.h"

static const char *test_file_name = "test.bin";

/*
1) Добавление типа без атрибутов в пустой файл
2) Добавление типа с атрибутами в пустой файл
3) Добавление типа без атрибутов в файл с одной заполненной секцией
4) Добавление типа с атрибутами в файл с одной заполненной секцией
*/

status_t File_AddTypeIntoEmptyFile_Successful()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    TYPE_INIT(V_type, "V");
    file_add_type(file, V_type);

    type_dtor(V_type);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_file_add_type()
{
    assert(File_AddTypeIntoEmptyFile_Successful() == OK);
}