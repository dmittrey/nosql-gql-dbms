#include <assert.h>

#include "string.h"

#include "memory/file/file.h"

#include "memory/json/json.h"
#include "physical/json/entity.h"

#include "memory/section/extents.h"

/*
Проверять:
TODO Перекидывание ссылок отца!!!

1) Удалить строку
2) Удалить инт
3) Удалить бул
4) Удалить флот
5) Удалить ноду первого уровня
6) Удалить ноду второго уровня
7) Удалить ноду третьего уровня
8) Удалить ноду, находяющуюся в разных секциях
*/

static const char *test_file_name = "test.bin";

status_t File_DeleteString_ShiftPtrsAndClear()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    STR_INIT(str, "example");
    JSON_VALUE_INIT(TYPE_STRING, json, "value", str);

    fileoff_t wrt_addr;
    DO_OR_FAIL(file_write(file, json, 0, &wrt_addr));

    DO_OR_FAIL(file_delete(file, wrt_addr));

    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t));
    assert(file->f_extent->header.lst_itm_ptr == sizeof(sect_head_entity_t));
    assert(file->f_extent->header.fst_rec_ptr == SECTION_SIZE);
    assert(file->f_extent->header.next_ptr == 0);
    assert(file->f_extent->header.sect_off == sizeof(file_head_t));

    void *temp = memset(my_malloc(entity_t), 0, sizeof(entity_t));

    entity_t *del_entity = entity_new();
    sect_ext_rd_itm(file->f_extent, wrt_addr, del_entity);

    assert(memcmp(temp, del_entity, sizeof(entity_t)) == 0);

    free(temp);
    json_dtor(json);

    entity_dtor(del_entity);

    file_dtor(file);
    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t File_DeleteInt_ShiftPtrsAndClear()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    JSON_VALUE_INIT(TYPE_INT32, json, "value", 5);

    fileoff_t wrt_addr;
    DO_OR_FAIL(file_write(file, json, 0, &wrt_addr));

    DO_OR_FAIL(file_delete(file, wrt_addr));

    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t));
    assert(file->f_extent->header.lst_itm_ptr == sizeof(sect_head_entity_t));
    assert(file->f_extent->header.fst_rec_ptr == SECTION_SIZE);
    assert(file->f_extent->header.next_ptr == 0);
    assert(file->f_extent->header.sect_off == sizeof(file_head_t));

    void *temp = memset(my_malloc(entity_t), 0, sizeof(entity_t));

    entity_t *del_entity = entity_new();
    sect_ext_rd_itm(file->f_extent, wrt_addr, del_entity);

    assert(memcmp(temp, del_entity, sizeof(entity_t)) == 0);

    free(temp);
    json_dtor(json);

    entity_dtor(del_entity);

    file_dtor(file);
    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t File_DeleteBool_ShiftPtrsAndClear()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    JSON_VALUE_INIT(TYPE_BOOL, json, "value", true);

    fileoff_t wrt_addr;
    DO_OR_FAIL(file_write(file, json, 0, &wrt_addr));

    DO_OR_FAIL(file_delete(file, wrt_addr));

    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t));
    assert(file->f_extent->header.lst_itm_ptr == sizeof(sect_head_entity_t));
    assert(file->f_extent->header.fst_rec_ptr == SECTION_SIZE);
    assert(file->f_extent->header.next_ptr == 0);
    assert(file->f_extent->header.sect_off == sizeof(file_head_t));

    void *temp = memset(my_malloc(entity_t), 0, sizeof(entity_t));

    entity_t *del_entity = entity_new();
    sect_ext_rd_itm(file->f_extent, wrt_addr, del_entity);

    assert(memcmp(temp, del_entity, sizeof(entity_t)) == 0);

    free(temp);
    json_dtor(json);

    entity_dtor(del_entity);

    file_dtor(file);
    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t File_DeleteFloat_ShiftPtrsAndClear()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    JSON_VALUE_INIT(TYPE_FLOAT, json, "value", 5.5);

    fileoff_t wrt_addr;
    DO_OR_FAIL(file_write(file, json, 0, &wrt_addr));

    DO_OR_FAIL(file_delete(file, wrt_addr));

    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t));
    assert(file->f_extent->header.lst_itm_ptr == sizeof(sect_head_entity_t));
    assert(file->f_extent->header.fst_rec_ptr == SECTION_SIZE);
    assert(file->f_extent->header.next_ptr == 0);
    assert(file->f_extent->header.sect_off == sizeof(file_head_t));

    void *temp = memset(my_malloc(entity_t), 0, sizeof(entity_t));

    entity_t *del_entity = entity_new();
    sect_ext_rd_itm(file->f_extent, wrt_addr, del_entity);

    assert(memcmp(temp, del_entity, sizeof(entity_t)) == 0);

    free(temp);
    json_dtor(json);

    entity_dtor(del_entity);

    file_dtor(file);
    fclose(filp);
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
//TODO Add asserts
status_t File_DeleteFirstLevelFromObjectNode_ShiftPtrsAndClearObject()
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

    DO_OR_FAIL(file_delete(file, wrt_addr));

    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t));
    assert(file->f_extent->header.lst_itm_ptr == sizeof(sect_head_entity_t));
    assert(file->f_extent->header.fst_rec_ptr == SECTION_SIZE);
    assert(file->f_extent->header.next_ptr == 0);
    assert(file->f_extent->header.sect_off == sizeof(file_head_t));

    void *del_memory = my_malloc_array(entity_t, 5);
    sect_ext_rd_rec(file->f_extent, wrt_addr, sizeof(entity_t) * 5, del_memory);
    void *cmp_memory = memset(my_malloc_array(entity_t, 5), 0, sizeof(entity_t) * 5);
    memcmp(del_memory, cmp_memory, sizeof(entity_t) * 5);

    free(del_memory);
    free(cmp_memory);
    json_dtor(info_json);

    file_dtor(file);
    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

/*
"info" : {
    "city" : {
        "location" : "Moscow",
        "amount" : 50000
    },
    "flag": true
}
*/
//TODO Add asserts
status_t File_DeleteSecondLevelFromObjectNode_ShiftPtrsAndClearObject()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    STR_INIT(location_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, location_json, "location", location_str);
    ENTITY_INIT(location_entity, location_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_INT32, amount_json, "amount", 50000);
    ENTITY_INIT(amount_entity, amount_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, city_json, "city", NULL);
    ENTITY_INIT(city_entity, city_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_BOOL, flag_json, "flag", true);
    ENTITY_INIT(flag_entity, flag_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, info_json, "info", NULL);
    ENTITY_INIT(info_entity, info_json, 0, 0, 0);

    json_add_son(city_json, location_json);
    json_add_son(city_json, amount_json);

    json_add_son(info_json, city_json);
    json_add_son(info_json, flag_json);

    fileoff_t wrt_addr;
    DO_OR_FAIL(file_write(file, info_json, 0, &wrt_addr));

    json_t *info_o_json = json_new();
    entity_t *info_o_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, wrt_addr), info_o_entity, info_o_json));
    DO_OR_FAIL(file_delete(file, info_o_entity->fam_addr.son_ptr));

    json_dtor(info_json);

    file_dtor(file);
    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

//TODO Add asserts
status_t File_DeleteThirdLevelFromObjectNode_ShiftPtrsAndClearObject()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    STR_INIT(location_str, "Moscow");
    JSON_VALUE_INIT(TYPE_STRING, location_json, "location", location_str);
    ENTITY_INIT(location_entity, location_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_INT32, amount_json, "amount", 50000);
    ENTITY_INIT(amount_entity, amount_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, city_json, "city", NULL);
    ENTITY_INIT(city_entity, city_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_BOOL, flag_json, "flag", true);
    ENTITY_INIT(flag_entity, flag_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, info_json, "info", NULL);
    ENTITY_INIT(info_entity, info_json, 0, 0, 0);

    json_add_son(city_json, location_json);
    json_add_son(city_json, amount_json);

    json_add_son(info_json, city_json);
    json_add_son(info_json, flag_json);

    fileoff_t wrt_addr;
    DO_OR_FAIL(file_write(file, info_json, 0, &wrt_addr));

    DO_OR_FAIL(file_delete(file, wrt_addr));




    json_dtor(info_json);

    file_dtor(file);
    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t File_DeleteFirstLevelFromObjectNodeFromVariousSections_ShiftPtrsAndClearObject()
{

}

void test_file_delete()
{
    assert(File_DeleteString_ShiftPtrsAndClear() == OK);
    assert(File_DeleteInt_ShiftPtrsAndClear() == OK);
    assert(File_DeleteBool_ShiftPtrsAndClear() == OK);
    assert(File_DeleteFloat_ShiftPtrsAndClear() == OK);
    assert(File_DeleteFirstLevelFromObjectNode_ShiftPtrsAndClearObject() == OK);
    assert(File_DeleteSecondLevelFromObjectNode_ShiftPtrsAndClearObject() == OK);
    assert(File_DeleteThirdLevelFromObjectNode_ShiftPtrsAndClearObject() == OK);
    // assert(File_DeleteFirstLevelFromObjectNodeFromVariousSections_ShiftPtrsAndClearObject() == OK);
}