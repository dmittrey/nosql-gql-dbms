#include <assert.h>
#include <string.h>

#include "memory/string.h"

#include "memory/file/file.h"

#include "memory/section/extents.h"

#include "physical/json/entity.h"
#include "physical/section/extents.h"

static const char *test_file_name = "test.bin";

/*
1) Выборка из нескольких элементов с несколькими условиями
2) Выборка из нескольких элементов с одним условием
3) Выборка из одного элемента с одним условием
4) Выборка из пустоты
*/

static status_t SectionExtents_FindFromSeveralQSeveral_ReturnSeveral()
{
    FILE *filp = fopen(test_file_name, "w+");

    STR_INIT(prev_location_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, prev_location_json, "location", prev_location_str);

    JSON_VALUE_INIT(TYPE_INT32, prev_amount_json, "amount", 50000);
    JSON_VALUE_INIT(TYPE_INT32, prev_amount_json_1, "amount", 50000);

    JSON_VALUE_INIT(TYPE_OBJECT, prev_city_json, "city", NULL);

    json_add_son(prev_city_json, prev_location_json);
    json_add_son(prev_city_json, prev_amount_json);

    JSON_VALUE_INIT(TYPE_OBJECT, prev_info_json, "info", NULL);

    json_add_son(prev_info_json, prev_city_json);
    json_add_son(prev_info_json, prev_amount_json_1);

    file_t *file = file_new();
    file_ctor(file, filp);

    fileoff_t save_json_fileoff;
    DO_OR_FAIL(file_write(file, prev_info_json, 0, &save_json_fileoff));

    STR_INIT(amount_q_key, "amount");
    QUERY_ITEM_INIT(TYPE_INT32, amount_query, amount_q_key, 50000);

    STR_INIT(location_q_key, "location");
    STR_INIT(location_q_val, "Moscow");
    QUERY_ITEM_INIT(TYPE_STRING, location_query, location_q_key, location_q_val);

    query_t *query = query_new();
    query_item_add(query, amount_query);
    query_item_add(query, location_query);

    list_json_t *collection = list_json_t_new();
    DO_OR_FAIL(sect_ext_find(file->f_extent, query, collection));

    assert(collection->count == 3);
    assert(json_cmp(collection->head, prev_amount_json) == 0);
    list_json_t_del_fst(collection);
    assert(json_cmp(collection->head, prev_location_json) == 0);
    list_json_t_del_fst(collection);
    assert(json_cmp(collection->head, prev_amount_json) == 0);

    query_dtor(query);

    list_json_t_dtor(collection);

    json_dtor(prev_info_json);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t SectionExtents_FindFromSeveral_ReturnSeveral()
{
    FILE *filp = fopen(test_file_name, "w+");

    STR_INIT(prev_location_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, prev_location_json, "location", prev_location_str);

    JSON_VALUE_INIT(TYPE_INT32, prev_amount_json, "amount", 50000);
    JSON_VALUE_INIT(TYPE_INT32, prev_amount_json_1, "amount", 50000);

    JSON_VALUE_INIT(TYPE_OBJECT, prev_city_json, "city", NULL);

    json_add_son(prev_city_json, prev_location_json);
    json_add_son(prev_city_json, prev_amount_json_1);

    JSON_VALUE_INIT(TYPE_OBJECT, prev_info_json, "info", NULL);

    json_add_son(prev_info_json, prev_city_json);
    json_add_son(prev_info_json, prev_amount_json);

    file_t *file = file_new();
    file_ctor(file, filp);

    fileoff_t save_json_fileoff;
    DO_OR_FAIL(file_write(file, prev_info_json, 0, &save_json_fileoff));

    STR_INIT(amount_q_key, "amount");
    QUERY_ITEM_INIT(TYPE_INT32, amount_query, amount_q_key, 50000);

    query_t *query = query_new();
    query_item_add(query, amount_query);

    list_json_t *collection = list_json_t_new();
    DO_OR_FAIL(sect_ext_find(file->f_extent, query, collection));

    assert(json_cmp(collection->head, prev_amount_json) == 0);
    assert(json_cmp(collection->tail, prev_amount_json) == 0);
    assert(collection->count == 2);

    query_dtor(query);

    list_json_t_dtor(collection);

    json_dtor(prev_info_json);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t SectionExtents_FindFromOne_ReturnOne()
{
    FILE *filp = fopen(test_file_name, "w+");

    STR_INIT(prev_location_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, prev_location_json, "location", prev_location_str);

    JSON_VALUE_INIT(TYPE_INT32, prev_amount_json, "amount", 50000);

    JSON_VALUE_INIT(TYPE_OBJECT, prev_city_json, "city", NULL);

    json_add_son(prev_city_json, prev_location_json);
    json_add_son(prev_city_json, prev_amount_json);

    JSON_VALUE_INIT(TYPE_OBJECT, prev_info_json, "info", NULL);

    json_add_son(prev_info_json, prev_city_json);

    file_t *file = file_new();
    file_ctor(file, filp);

    fileoff_t save_json_fileoff;
    DO_OR_FAIL(file_write(file, prev_info_json, 0, &save_json_fileoff));

    STR_INIT(amount_q_key, "amount");
    QUERY_ITEM_INIT(TYPE_INT32, amount_query, amount_q_key, 50000);

    query_t *query = query_new();
    query_item_add(query, amount_query);

    list_json_t *collection = list_json_t_new();
    DO_OR_FAIL(sect_ext_find(file->f_extent, query, collection));

    assert(json_cmp(collection->head, prev_amount_json) == 0);
    assert(json_cmp(collection->tail, prev_amount_json) == 0);
    assert(collection->count == 1);

    query_dtor(query);

    list_json_t_dtor(collection);

    json_dtor(prev_info_json);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t SectionExtents_FindFromEmpty_ReturnEmpty()
{
    FILE *filp = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, filp);

    STR_INIT(amount_q_key, "amount");
    QUERY_ITEM_INIT(TYPE_INT32, amount_query, amount_q_key, 50000);

    query_t *query = query_new();
    query_item_add(query, amount_query);

    list_json_t *collection = list_json_t_new();
    DO_OR_FAIL(sect_ext_find(extents, query, collection));

    assert(collection->head == NULL);
    assert(collection->tail == NULL);
    assert(collection->count == 0);

    query_dtor(query);

    list_json_t_dtor(collection);

    sect_ext_dtor(extents);

    fclose(filp);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_extents_find()
{
    assert(SectionExtents_FindFromSeveralQSeveral_ReturnSeveral() == OK);
    assert(SectionExtents_FindFromSeveral_ReturnSeveral() == OK);
    assert(SectionExtents_FindFromOne_ReturnOne() == OK);
    assert(SectionExtents_FindFromEmpty_ReturnEmpty() == OK);
}