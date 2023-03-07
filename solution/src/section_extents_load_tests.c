#include <assert.h>
#include <string.h>

#include "memory/json/json_col.h"

#include "memory/file/file.h"

#include "memory/section/extents.h"

#include "physical/json/entity.h"
#include "physical/section/extents.h"

static const char *test_file_name = "test.bin";

static status_t SectionExtents_LoadEmpty_ReturnEmptyCol()
{
    FILE *filp = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, filp);

    json_col_t *collection = json_col_new();
    DO_OR_FAIL(sect_ext_load(extents, collection));

    assert(collection->f_json == NULL);
    assert(collection->l_json == NULL);

    json_col_dtor(collection);

    sect_ext_dtor(extents);

    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t SectionExtents_Load3LvlObject_ReturnColWithFiveEls()
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
    json_col_del_fst(collection);
    assert(json_cmp(collection->f_json, prev_city_json) == 0);
    json_col_del_fst(collection);
    assert(json_cmp(collection->f_json, prev_flag_json) == 0);
    json_col_del_fst(collection);
    assert(json_cmp(collection->f_json, prev_location_json) == 0);
    json_col_del_fst(collection);
    assert(json_cmp(collection->f_json, prev_amount_json) == 0);
    json_col_del_fst(collection);

    json_col_dtor(collection);

    json_dtor(prev_info_json);

    entity_dtor(prev_location_entity);
    entity_dtor(prev_amount_entity);
    entity_dtor(prev_city_entity);
    entity_dtor(prev_flag_entity);
    entity_dtor(prev_info_entity);

    file_dtor(file);
    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_extents_load()
{
    assert(SectionExtents_LoadEmpty_ReturnEmptyCol() == OK);
    assert(SectionExtents_Load3LvlObject_ReturnColWithFiveEls() == OK);
}