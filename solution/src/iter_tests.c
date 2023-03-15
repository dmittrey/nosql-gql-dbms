#include <assert.h>

#include "memory/file/file.h"

#include "memory/json/iter.h"

static const char *test_file_name = "test.bin";

/*
1) Сделать выборку из нуля по query вызвать get(вернуть Null)
2) Сделать выборку из 1 по 1
3) Сделать выборку из нескольких по 1

Суть в том, чтобы поиграться с get,
*/

status_t Iter_FetchFromEmptyFile_IterContainsNothing()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    struct iter_t *iter = iter_new();

    STR_INIT(amount_q_key, "amount");
    QUERY_ITEM_INIT(TYPE_INT32, amount_query, amount_q_key, 50000);

    query_t *query = query_new();
    query_item_add(query, amount_query);

    iter_ctor(iter, file, query);

    assert(iter_get(iter) == NULL);
    assert(iter_is_avail(iter) == false);

    iter_dtor(iter);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t Iter_FetchFromFileWithOneTargetJson_IterContainsOneElem()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    STR_INIT(prev_location_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, prev_location_json, "location", prev_location_str);

    JSON_VALUE_INIT(TYPE_INT32, prev_amount_json, "amount", 50000);

    JSON_VALUE_INIT(TYPE_INT32, prev_amount_json_1, "amount_1", 50000);

    JSON_VALUE_INIT(TYPE_OBJECT, prev_city_json, "city", NULL);

    json_add_son(prev_city_json, prev_location_json);
    json_add_son(prev_city_json, prev_amount_json_1);

    JSON_VALUE_INIT(TYPE_OBJECT, prev_info_json, "info", NULL);

    json_add_son(prev_info_json, prev_city_json);
    json_add_son(prev_info_json, prev_amount_json);

    fileoff_t save_json_fileoff;
    DO_OR_FAIL(file_write(file, prev_info_json, 0, 0, &save_json_fileoff));

    STR_INIT(amount_q_key, "amount");
    QUERY_ITEM_INIT(TYPE_INT32, amount_query, amount_q_key, 50000);

    query_t *query = query_new();
    query_item_add(query, amount_query);

    struct iter_t *iter = iter_new();
    iter_ctor(iter, file, query);

    assert(iter_is_avail(iter) == true);
    assert(json_cmp(iter_get(iter), prev_info_json) == 0);

    iter_next(iter);
    assert(iter_is_avail(iter) == false);
    assert(iter_get(iter) == NULL);

    json_dtor(prev_info_json);

    iter_dtor(iter);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t Iter_FetchFromFileWithTwoTargetJson_IterContainsTwoElem()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

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

    fileoff_t save_json_fileoff;
    DO_OR_FAIL(file_write(file, prev_info_json, 0, 0, &save_json_fileoff));

    STR_INIT(amount_q_key, "amount");
    QUERY_ITEM_INIT(TYPE_INT32, amount_query, amount_q_key, 50000);

    query_t *query = query_new();
    query_item_add(query, amount_query);

    struct iter_t *iter = iter_new();
    iter_ctor(iter, file, query);

    assert(iter_is_avail(iter) == true);
    assert(json_cmp(iter_get(iter), prev_info_json) == 0);

    iter_next(iter);
    assert(iter_is_avail(iter) == true);
    assert(json_cmp(iter_get(iter), prev_city_json) == 0);

    iter_next(iter);
    assert(iter_is_avail(iter) == false);
    assert(iter_get(iter) == NULL);

    json_dtor(prev_info_json);

    iter_dtor(iter);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_iter()
{
    assert(Iter_FetchFromEmptyFile_IterContainsNothing() == OK);
    assert(Iter_FetchFromFileWithOneTargetJson_IterContainsOneElem() == OK);
    assert(Iter_FetchFromFileWithTwoTargetJson_IterContainsTwoElem() == OK);
}