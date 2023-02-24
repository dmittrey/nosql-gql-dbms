/*
По базе прочитать все 4 типа и object
*/
#include <assert.h>
#include <string.h>

#include "memory/section/extents.h"

#include "physical/json/entity.h"
#include "physical/section/extents.h"

static const char *test_file_name = "test.bin";

static status_t SectionExtents_ReadInt32Json_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, json, "value", 5);

    fileoff_t save_json_addr;
    fileoff_t parent_json_addr = 500;

    DO_OR_FAIL(sect_ext_write(extents, json, parent_json_addr, 0, 0, &save_json_addr));

    json_t *r_json = json_new();
    DO_OR_FAIL(sect_ext_read(extents, save_json_addr, r_json));

    return OK;
}

static status_t SectionExtents_ReadFloatJson_Successful()
{
    
}

static status_t SectionExtents_ReadBoolJson_Successful()
{
    
}

static status_t SectionExtents_ReadStringJson_Successful()
{
    
}

static status_t SectionExtents_ReadObjectJson_Successful()
{
    
}

void test_extents_read()
{
}
