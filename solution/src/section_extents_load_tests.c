#include <assert.h>
#include <string.h>

#include "memory/json/it_json.h"

#include "memory/section/extents.h"

#include "physical/json/entity.h"
#include "physical/section/extents.h"

static const char *test_file_name = "test.bin";

static status_t SectionExtents_LoadType_ReturnItJsonWithObject()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    DO_OR_FAIL(sect_ext_ctor(extents, 0, file));

    JSON_VALUE_INIT(TYPE_INT32, json, "json", 5);
    ENTITY_INIT(entity, json, 500, 200, 300);

    sectoff_t save_json_sectoff;
    DO_OR_FAIL(sect_ext_write(extents, json, entity, &save_json_sectoff));
    DO_OR_FAIL(sect_ext_write(extents, json, entity, &save_json_sectoff));
    DO_OR_FAIL(sect_ext_write(extents, json, entity, &save_json_sectoff));
    DO_OR_FAIL(sect_ext_write(extents, json, entity, &save_json_sectoff));

    it_json_t *it_json = it_json_new();
    DO_OR_FAIL(sect_ext_load(extents, it_json));

    while (it_json != NULL)
    {
        json_print(it_json->json);
        it_json = it_json->next;
    }
    
    return OK;
}

void test_extents_load()
{
    assert(SectionExtents_LoadType_ReturnItJsonWithObject() == OK);
}