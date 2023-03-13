#include <assert.h>

#include "string.h"

#include "memory/file/file.h"

#include "memory/json/json.h"
#include "physical/json/entity.h"

#include "memory/section/extents.h"

/*
1) Обновить строку
2) Обновить инт
3) Обновить бул
4) Обновить флот
5) Обновить двух уровневый объект
6) Обновить трех уровневый объект
7) Обновить трех уровневый объект в разных секциях
8) Обновить объект на больший при отсутствии места для большего - должен записать в конец
9) Обновить обхект на больший при отсуствии места в секции - должен создать секцию и записать в конец
*/

static const char *test_file_name = "test.bin";

static status_t File_UpdateType_Update(json_t *const json, json_t *const new_json)
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    fileoff_t wrt_addr;
    DO_OR_FAIL(file_write(file, json, 0, &wrt_addr));

    DO_OR_FAIL(file_update(file, wrt_addr, new_json, 0, false, &wrt_addr));

    json_t * o_json = json_new();
    DO_OR_FAIL(file_read(file, wrt_addr, o_json));
    assert(json_cmp(o_json, new_json) == 0);

    json_dtor(o_json);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

/*
{
    "obj": {
        "son" : "value"
    },
    "bro" : "example"
}

{
    "obj": {
        "bro" : "example"
    },
    "son" : "value"
}
*/
static const status_t File_Update2LvlObject_Successful()
{
    STR_INIT(prev_son_str, "value");
    JSON_VALUE_INIT(TYPE_STRING, prev_son_json, "son", prev_son_str);
    ENTITY_INIT(prev_son_entity, prev_son_json, 0, 0, 0);

    STR_INIT(prev_bro_str, "example");
    JSON_VALUE_INIT(TYPE_STRING, prev_bro_json, "bro", prev_bro_str);
    ENTITY_INIT(prev_bro_entity, prev_bro_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, prev_obj_json, "obj", NULL);
    ENTITY_INIT(prev_obj_entity, prev_obj_json, 0, 0, 0);

    json_add_bro(prev_obj_json, prev_bro_json);
    json_add_son(prev_obj_json, prev_son_json);

    STR_INIT(new_son_str, "value");
    JSON_VALUE_INIT(TYPE_STRING, new_son_json, "son", new_son_str);
    ENTITY_INIT(new_son_entity, new_son_json, 0, 0, 0);

    STR_INIT(new_bro_str, "example");
    JSON_VALUE_INIT(TYPE_STRING, new_bro_json, "bro", new_bro_str);
    ENTITY_INIT(new_bro_entity, new_bro_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, new_obj_json, "obj", NULL);
    ENTITY_INIT(new_obj_entity, new_obj_json, 0, 0, 0);

    json_add_bro(new_obj_json, new_son_json);
    json_add_son(new_obj_json, new_bro_json);

    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    fileoff_t wrt_addr;
    DO_OR_FAIL(file_write(file, prev_obj_json, 0, &wrt_addr));

    DO_OR_FAIL(file_update(file, wrt_addr, new_obj_json, 0, true, &wrt_addr));

    json_t *o_json = json_new();
    DO_OR_FAIL(file_read(file, wrt_addr, o_json));
    // assert(json_cmp(&o_json, new_obj_json) == 0);
    // assert(json_cmp(o_json.bro, new_obj_json->bro) == 0);
    // assert(json_cmp(o_json.son, new_obj_json->son) == 0);

    json_dtor(o_json->bro);
    json_dtor(o_json);
    json_dtor(prev_obj_json);
    json_dtor(prev_bro_json);
    json_dtor(new_obj_json);
    json_dtor(new_son_json);

    entity_dtor(prev_son_entity);
    entity_dtor(prev_bro_entity);
    entity_dtor(prev_obj_entity);
    entity_dtor(new_son_entity);
    entity_dtor(new_bro_entity);
    entity_dtor(new_obj_entity);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

/*
"info" : {
    "city" : {
        "location" : "Moscow",
        "amount" : "50000"
    },
    "flag": true
}
*/
/*
"info" : {
    "city" : "Moscow"
}
*/
static const status_t File_Update3LvlObject_Successful()
{
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


    STR_INIT(new_city_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, new_city_json, "city", new_city_str);
    ENTITY_INIT(new_city_entity, new_city_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, new_info_json, "info", NULL);
    ENTITY_INIT(new_info_entity, new_info_json, 0, 0, 0);

    json_add_son(new_info_json, new_city_json);
    
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    fileoff_t wrt_addr;
    DO_OR_FAIL(file_write(file, prev_info_json, 0, &wrt_addr));

    DO_OR_FAIL(file_update(file, wrt_addr, new_info_json, 0, false, &wrt_addr));

    json_t * o_json = json_new();
    DO_OR_FAIL(file_read(file, wrt_addr, o_json));
    assert(json_cmp(o_json, new_info_json) == 0);
    assert(json_cmp(o_json->son, new_info_json->son) == 0);
    assert(o_json->bro == NULL);

    json_dtor(o_json);
    json_dtor(new_info_json);
    json_dtor(prev_info_json);

    entity_dtor(prev_location_entity);
    entity_dtor(prev_amount_entity);
    entity_dtor(prev_city_entity);
    entity_dtor(prev_flag_entity);
    entity_dtor(prev_info_entity);

    entity_dtor(new_city_entity);
    entity_dtor(new_info_entity);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}


void test_file_update()
{
    STR_INIT(location_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, string_json, "location", location_str);
    JSON_VALUE_INIT(TYPE_INT32, int_json, "int", 50000);
    JSON_VALUE_INIT(TYPE_FLOAT, float_json, "float", 5.5);
    JSON_VALUE_INIT(TYPE_BOOL, bool_json, "bool", 50000);
    JSON_VALUE_INIT(TYPE_OBJECT, object_json, "object", NULL);

    assert(File_UpdateType_Update(int_json, string_json) == OK);
    assert(File_UpdateType_Update(float_json, string_json) == OK);
    assert(File_UpdateType_Update(bool_json, string_json) == OK);
    assert(File_UpdateType_Update(object_json, string_json) == OK);
    assert(File_UpdateType_Update(string_json, float_json) == OK);

    json_dtor(string_json);
    json_dtor(int_json);
    json_dtor(float_json);
    json_dtor(bool_json);
    json_dtor(object_json);

    assert(File_Update2LvlObject_Successful() == OK);

    assert(File_Update3LvlObject_Successful() == OK);
}