#include <assert.h>
#include <string.h>

#include "memory/json/json_col.h"

#include "memory/file/file.h"

#include "memory/section/extents.h"

#include "physical/json/entity.h"
#include "physical/section/extents.h"

static const char *test_file_name = "test.bin";

/*
1) Проверить считывание коллекций всех типов(каждого по одному)
2) Проверить считывание всех элементов отдельно составного объекта записанного в одну секцию
*/

static status_t SectionExtents_LoadType_ReturnItJsonWithObject()
{
    FILE *filp = fopen(test_file_name, "w+");

    STR_INIT(prev_location_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, prev_location_json, "location", prev_location_str);
    ENTITY_INIT(prev_location_entity, prev_location_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_INT32, prev_amount_json, "amount", 50000);
    ENTITY_INIT(prev_amount_entity, prev_amount_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, prev_city_json, "city", NULL);
    ENTITY_INIT(prev_city_entity, prev_city_json, 0, 0, 0);

    json_add_son(prev_city_json, prev_location_json);
    json_add_son(prev_city_json, prev_amount_json);

    JSON_VALUE_INIT(TYPE_BOOL, prev_flag_json, "flag", true);
    ENTITY_INIT(prev_flag_entity, prev_flag_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, prev_info_json, "info", NULL);
    ENTITY_INIT(prev_info_entity, prev_info_json, 0, 0, 0);

    json_add_son(prev_info_json, prev_city_json);
    json_add_son(prev_info_json, prev_flag_json);

    file_t *file = file_new();
    file_ctor(file, filp);

    fileoff_t save_json_fileoff;
    DO_OR_FAIL(file_write(file, prev_info_json, 0, &save_json_fileoff));

    json_col_t *collection = json_col_new();
    DO_OR_FAIL(sect_ext_load(file->f_extent, collection));

    assert(json_cmp(collection->f_json, prev_info_json) == 0);
    
    assert(json_cmp(collection->f_json, prev_amount_json) == 0);
    assert(json_cmp(collection->f_json, prev_location_json) == 0);
    assert(json_cmp(collection->f_json, prev_flag_json) == 0);
    assert(json_cmp(collection->f_json, prev_city_json) == 0);

    while (collection->f_json != NULL)
    {
        json_print(collection->f_json);
        printf(" | ");
        collection->f_json = collection->f_json->next;
    }

    return OK;
}

void test_extents_load()
{
    assert(SectionExtents_LoadType_ReturnItJsonWithObject() == OK);
}