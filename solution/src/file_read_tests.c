#include <assert.h>

#include "string.h"

#include "memory/file/file.h"

#include "memory/json/json.h"
#include "physical/json/entity.h"

#include "memory/section/extents.h"

/*
1) Чтение инта
2) Чтение строки
3) Чтение була
4) Чтение флота
5) Чтение с нулевого поинтера
6) Чтение составного объекта в одной секции
7) Чтение составного объекта из разных секции
8) Чтение трехуровнего объекта
*/

static const char *test_file_name = "test.bin";

static status_t File_ReadInt_Successful()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    JSON_VALUE_INIT(TYPE_INT32, json, "value", 5);

    fileoff_t wrt_addr;
    DO_OR_FAIL(file_write(file, json, 0, &wrt_addr));

    json_t* o_json = json_new();
    DO_OR_FAIL(file_read(file, wrt_addr, o_json));

    assert(o_json->key->cnt == json->key->cnt);
    assert(strncmp(o_json->key->val, json->key->val, o_json->key->cnt) == 0);
    assert(o_json->value.int32_val == json->value.int32_val);
    assert(o_json->dad == NULL);
    assert(o_json->bro == NULL);
    assert(o_json->son == NULL);
    assert(o_json->type == json->type);

    json_dtor(json);
    json_dtor(o_json);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t File_ReadString_Successful()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    STR_INIT(str, "example");
    JSON_VALUE_INIT(TYPE_STRING, json, "value", str);

    fileoff_t wrt_addr;
    DO_OR_FAIL(file_write(file, json, 0, &wrt_addr));

    json_t* o_json = json_new();
    DO_OR_FAIL(file_read(file, wrt_addr, o_json));

    assert(o_json->key->cnt == json->key->cnt);
    assert(strncmp(o_json->key->val, json->key->val, o_json->key->cnt) == 0);
    assert(o_json->value.string_val->cnt == json->value.string_val->cnt);
    assert(strncmp(o_json->value.string_val->val, json->value.string_val->val, o_json->value.string_val->cnt) == 0);
    assert(o_json->dad == NULL);
    assert(o_json->bro == NULL);
    assert(o_json->son == NULL);
    assert(o_json->type == json->type);

    json_dtor(json);
    json_dtor(o_json);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t File_ReadBool_Successful()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    JSON_VALUE_INIT(TYPE_BOOL, json, "value", true);

    fileoff_t wrt_addr;
    DO_OR_FAIL(file_write(file, json, 0, &wrt_addr));

    json_t* o_json = json_new();
    DO_OR_FAIL(file_read(file, wrt_addr, o_json));

    assert(o_json->key->cnt == json->key->cnt);
    assert(strncmp(o_json->key->val, json->key->val, o_json->key->cnt) == 0);
    assert(o_json->value.bool_val == json->value.bool_val);
    assert(o_json->dad == NULL);
    assert(o_json->bro == NULL);
    assert(o_json->son == NULL);
    assert(o_json->type == json->type);

    json_dtor(json);
    json_dtor(o_json);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t File_ReadFloat_Successful()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    JSON_VALUE_INIT(TYPE_FLOAT, json, "value", 5.5);

    fileoff_t wrt_addr;
    DO_OR_FAIL(file_write(file, json, 0, &wrt_addr));

    json_t* o_json = json_new();
    DO_OR_FAIL(file_read(file, wrt_addr, o_json));

    assert(o_json->key->cnt == json->key->cnt);
    assert(strncmp(o_json->key->val, json->key->val, o_json->key->cnt) == 0);
    assert(o_json->value.float_val == json->value.float_val);
    assert(o_json->dad == NULL);
    assert(o_json->bro == NULL);
    assert(o_json->son == NULL);
    assert(o_json->type == json->type);

    json_dtor(json);
    json_dtor(o_json);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t File_ReadFromZeroPtr_Failed()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    JSON_VALUE_INIT(TYPE_FLOAT, json, "value", 5.5);

    fileoff_t wrt_addr;
    DO_OR_FAIL(file_write(file, json, 0, &wrt_addr));

    json_t* o_json = json_new();
    status_t read_status = file_read(file, 0, o_json);

    json_dtor(json);
    json_dtor(o_json);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return read_status;
}

/*
"city" : {
    "name" : "Moscow",
    "amount" : 50000
}
*/
static status_t File_ReadObjectWithComponentsFromSameSect_Successful()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    JSON_VALUE_INIT(TYPE_OBJECT, city_json, "city", NULL);
    STR_INIT(name_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, name_json, "name", name_str);
    JSON_VALUE_INIT(TYPE_INT32, amount_json, "amount", 50000);

    json_add_son(city_json, name_json);
    json_add_son(city_json, amount_json);

    fileoff_t wrt_addr;
    DO_OR_FAIL(file_write(file, city_json, 0, &wrt_addr));

    json_t* city_o_json = json_new();
    DO_OR_FAIL(file_read(file, wrt_addr, city_o_json));

    json_dtor(city_json);
    json_dtor(city_o_json);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t File_ReadObjectWithComponentsFromVariousSect_Successful()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    sect_ext_t* f_sect = sect_ext_new();
    file_add_sect_ext(file, f_sect);
    sect_head_shift_lst_itm_ptr(&f_sect->header, 8000);

    JSON_VALUE_INIT(TYPE_OBJECT, city_json, "city", NULL);
    STR_INIT(name_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, name_json, "name", name_str);
    JSON_VALUE_INIT(TYPE_INT32, amount_json, "amount", 50000);

    json_add_son(city_json, name_json);
    json_add_son(city_json, amount_json);

    fileoff_t wrt_addr;
    DO_OR_FAIL(file_write(file, city_json, 0, &wrt_addr));

    json_t* city_o_json = json_new();
    DO_OR_FAIL(file_read(file, wrt_addr, city_o_json));

    json_dtor(city_json);
    json_dtor(city_o_json);

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
static status_t File_ReadThreeLvlObject_Successful()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    STR_INIT(location_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, location_json, "location", location_str);
    JSON_VALUE_INIT(TYPE_INT32, amount_json, "amount", 50000);
    JSON_VALUE_INIT(TYPE_OBJECT, city_json, "city", NULL);
    JSON_VALUE_INIT(TYPE_BOOL, flag_json, "flag", true);
    JSON_VALUE_INIT(TYPE_OBJECT, info_json, "info", NULL);

    json_add_son(city_json, location_json);
    json_add_son(city_json, amount_json);

    json_add_son(info_json, city_json);
    json_add_son(info_json, flag_json);

    fileoff_t wrt_addr;
    DO_OR_FAIL(file_write(file, info_json, 0, &wrt_addr));

    json_t* info_o_json = json_new();
    DO_OR_FAIL(file_read(file, wrt_addr, info_o_json));

    json_dtor(info_json);
    json_dtor(info_o_json);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_file_read()
{
    assert(File_ReadInt_Successful() == OK);
    assert(File_ReadString_Successful() == OK);
    assert(File_ReadBool_Successful() == OK);
    assert(File_ReadFloat_Successful() == OK);
    assert(File_ReadFromZeroPtr_Failed() == FAILED);
    assert(File_ReadObjectWithComponentsFromSameSect_Successful() == OK);
    assert(File_ReadObjectWithComponentsFromVariousSect_Successful() == OK);
    assert(File_ReadThreeLvlObject_Successful() == OK);
}