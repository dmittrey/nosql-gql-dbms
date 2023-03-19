#include <assert.h>

static const char *test_file_name = "test.bin";

#include "section/extents.h"
#include "section/extents_p.h"

static Status SectionExtents_LoadEmpty_ReturnEmptyCol()
{
    FILE *filp = fopen(test_file_name, "w+");

    Sect_ext *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, filp);

    List_Pair_Json_Entity *collection = list_Pair_Json_Entity_new();
    DO_OR_FAIL(sect_ext_load(extents, collection));

    assert(collection->head == NULL);
    assert(collection->tail == NULL);
    assert(collection->count == 0);

    list_Pair_Json_Entity_dtor(collection);

    sect_ext_dtor(extents);

    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static Status SectionExtents_LoadObjects_ReturnColWithFiveEls()
{
    FILE *filp = fopen(test_file_name, "w+");

    STR_INIT(location_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, location_json, "location", location_str);
    ENTITY_INIT(location_entity, location_json, 0, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_INT32, amount_json, "amount", 50000);
    ENTITY_INIT(amount_entity, amount_json, 0, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, city_json, "city", NULL);
    ENTITY_INIT(city_entity, city_json, 0, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_BOOL, flag_json, "flag", true);
    ENTITY_INIT(flag_entity, flag_json, 0, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, info_json, "info", NULL);
    ENTITY_INIT(info_entity, info_json, 0, 0, 0, 0);

    Sect_ext *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, filp);

    Fileoff save_json_fileoff;
    DO_OR_FAIL(sect_ext_write(extents, location_json->key, json_val_ptr(location_json),
                              json_val_size(location_json), location_entity, &save_json_fileoff));
    DO_OR_FAIL(sect_ext_write(extents, amount_json->key, json_val_ptr(amount_json),
                              json_val_size(amount_json), amount_entity, &save_json_fileoff));
    DO_OR_FAIL(sect_ext_write(extents, city_json->key, json_val_ptr(city_json),
                              json_val_size(city_json), city_entity, &save_json_fileoff));
    DO_OR_FAIL(sect_ext_write(extents, flag_json->key, json_val_ptr(flag_json),
                              json_val_size(flag_json), flag_entity, &save_json_fileoff));
    DO_OR_FAIL(sect_ext_write(extents, info_json->key, json_val_ptr(info_json),
                              json_val_size(info_json), info_entity, &save_json_fileoff));

    List_Pair_Json_Entity *collection = list_Pair_Json_Entity_new();
    DO_OR_FAIL(sect_ext_load(extents, collection));

    assert(json_cmp(collection->head->f, location_json) == 0);
    assert(entity_cmp(collection->head->s, location_entity) == 0);
    list_Pair_Json_Entity_del_fst(collection);
    assert(json_cmp(collection->head->f, amount_json) == 0);
    assert(entity_cmp(collection->head->s, amount_entity) == 0);
    list_Pair_Json_Entity_del_fst(collection);
    assert(json_cmp(collection->head->f, city_json) == 0);
    assert(entity_cmp(collection->head->s, city_entity) == 0);
    list_Pair_Json_Entity_del_fst(collection);
    assert(json_cmp(collection->head->f, flag_json) == 0);
    assert(entity_cmp(collection->head->s, flag_entity) == 0);
    list_Pair_Json_Entity_del_fst(collection);
    assert(json_cmp(collection->head->f, info_json) == 0);
    assert(entity_cmp(collection->head->s, info_entity) == 0);
    list_Pair_Json_Entity_del_fst(collection);

    list_Pair_Json_Entity_dtor(collection);

    json_dtor(location_json);
    json_dtor(amount_json);
    json_dtor(city_json);
    json_dtor(flag_json);
    json_dtor(info_json);

    entity_dtor(location_entity);
    entity_dtor(amount_entity);
    entity_dtor(city_entity);
    entity_dtor(flag_entity);
    entity_dtor(info_entity);

    sect_ext_dtor(extents);

    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_extents_load()
{
    assert(SectionExtents_LoadEmpty_ReturnEmptyCol() == OK);
    assert(SectionExtents_LoadObjects_ReturnColWithFiveEls() == OK);
}