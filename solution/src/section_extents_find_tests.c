#include <assert.h>

#include "section/extents.h"
#include "section/extents_p.h"

static const char *test_file_name = "test.bin";

/*
1) Выборка из нескольких элементов с несколькими условиями
2) Выборка из нескольких элементов с одним условием
3) Выборка из одного элемента с одним условием
4) Выборка из пустоты
*/

static Status SectionExtents_FindFromSeveralQSeveral_ReturnSeveral()
{
    FILE *filp = fopen(test_file_name, "w+");

    Sect_ext *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, filp);

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

    STR_INIT(amount_q_key, "amount");
    QUERY_ITEM_INIT(TYPE_INT32, amount_query, amount_q_key, 50000);

    STR_INIT(location_q_key, "location");
    STR_INIT(location_q_val, "Moscow");
    QUERY_ITEM_INIT(TYPE_STRING, location_query, location_q_key, location_q_val);

    Query *query = query_new();
    query_item_add(query, amount_query);
    query_item_add(query, location_query);

    List_Pair_Json_Entity *collection = list_Pair_Json_Entity_new();
    DO_OR_FAIL(sect_ext_find(extents, query, collection));

    assert(collection->count == 2);
    assert(json_cmp(collection->head->f, location_json) == 0);
    assert(entity_cmp(collection->head->s, location_entity) == 0);
    list_Pair_Json_Entity_del_fst(collection);
    assert(json_cmp(collection->head->f, amount_json) == 0);
    assert(entity_cmp(collection->head->s, amount_entity) == 0);
    list_Pair_Json_Entity_del_fst(collection);

    query_dtor(query);

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

    list_Pair_Json_Entity_dtor(collection);

    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static Status SectionExtents_FindFromSeveralWithOneQuery_ReturnOne()
{
    FILE *filp = fopen(test_file_name, "w+");

    Sect_ext *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, filp);

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

    STR_INIT(amount_q_key, "amount");
    QUERY_ITEM_INIT(TYPE_INT32, amount_query, amount_q_key, 50000);

    Query *query = query_new();
    query_item_add(query, amount_query);

    List_Pair_Json_Entity *collection = list_Pair_Json_Entity_new();
    DO_OR_FAIL(sect_ext_find(extents, query, collection));

    assert(collection->count == 1);
    assert(json_cmp(collection->head->f, amount_json) == 0);
    assert(entity_cmp(collection->head->s, amount_entity) == 0);
    list_Pair_Json_Entity_del_fst(collection);

    query_dtor(query);

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

    list_Pair_Json_Entity_dtor(collection);

    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static Status SectionExtents_FindFromOne_ReturnOne()
{
    FILE *filp = fopen(test_file_name, "w+");

    Sect_ext *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, filp);

    JSON_VALUE_INIT(TYPE_INT32, amount_json, "amount", 50000);
    ENTITY_INIT(amount_entity, amount_json, 0, 0, 0, 0);

    Fileoff save_json_fileoff;
    DO_OR_FAIL(sect_ext_write(extents, amount_json->key, json_val_ptr(amount_json),
                              json_val_size(amount_json), amount_entity, &save_json_fileoff));

    STR_INIT(amount_q_key, "amount");
    QUERY_ITEM_INIT(TYPE_INT32, amount_query, amount_q_key, 50000);

    Query *query = query_new();
    query_item_add(query, amount_query);

    List_Pair_Json_Entity *collection = list_Pair_Json_Entity_new();
    DO_OR_FAIL(sect_ext_find(extents, query, collection));

    assert(collection->count == 1);
    assert(json_cmp(collection->head->f, amount_json) == 0);
    assert(entity_cmp(collection->head->s, amount_entity) == 0);
    list_Pair_Json_Entity_del_fst(collection);

    query_dtor(query);

    json_dtor(amount_json);

    entity_dtor(amount_entity);

    list_Pair_Json_Entity_dtor(collection);

    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static Status SectionExtents_FindFromEmpty_ReturnEmpty()
{
    FILE *filp = fopen(test_file_name, "w+");

    Sect_ext *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, filp);

    STR_INIT(amount_q_key, "amount");
    QUERY_ITEM_INIT(TYPE_INT32, amount_query, amount_q_key, 50000);

    Query *query = query_new();
    query_item_add(query, amount_query);

    List_Pair_Json_Entity *collection = list_Pair_Json_Entity_new();
    DO_OR_FAIL(sect_ext_find(extents, query, collection));

    assert(collection->head == NULL);
    assert(collection->tail == NULL);
    assert(collection->count == 0);

    query_dtor(query);

    list_Pair_Json_Entity_dtor(collection);

    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_extents_find()
{
    assert(SectionExtents_FindFromSeveralQSeveral_ReturnSeveral() == OK);
    assert(SectionExtents_FindFromSeveralWithOneQuery_ReturnOne() == OK);
    assert(SectionExtents_FindFromOne_ReturnOne() == OK);
    assert(SectionExtents_FindFromEmpty_ReturnEmpty() == OK);
}