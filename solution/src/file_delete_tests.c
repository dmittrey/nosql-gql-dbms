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
*/

static const char *test_file_name = "test.bin";

status_t File_DeleteType_ShiftPtrsAndClear(const json_t *const json)
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    ENTITY_INIT(entity, json, 0, 0, 0);

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
    DO_OR_FAIL(sect_ext_rd_itm(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, wrt_addr), del_entity));

    // Check item clear
    assert(memcmp(temp, del_entity, sizeof(entity_t)) == 0);

    // Check rec clear
    void *rec = my_malloc_array(char, entity_rec_size(entity));
    void *temp_rec_zr = memset(my_malloc_array(char, entity_rec_size(entity)), 0, entity_rec_size(entity));
    DO_OR_FAIL(sect_ext_rd_rec(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, SECTION_SIZE - entity_rec_size(entity)), entity_rec_size(entity), rec));
    assert(memcmp(rec, temp_rec_zr, entity_rec_size(entity)) == 0);

    free(temp);
    free(rec);
    free(temp_rec_zr);

    entity_dtor(del_entity);
    entity_dtor(entity);

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
status_t File_DeleteFirstLevelFromObjectNode_ShiftPtrsAndClearObject()
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

    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t));
    assert(file->f_extent->header.lst_itm_ptr == sizeof(sect_head_entity_t));
    assert(file->f_extent->header.fst_rec_ptr == SECTION_SIZE);
    assert(file->f_extent->header.next_ptr == 0);
    assert(file->f_extent->header.sect_off == sizeof(file_head_t));

    // Check clear items
    void *del_items_area = my_malloc_array(entity_t, 5);
    void *temp_items_zr = memset(my_malloc_array(entity_t, 5), 0, 5 * sizeof(entity_t));
    sect_ext_rd_rec(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, wrt_addr), sizeof(entity_t) * 5, del_items_area);
    assert(memcmp(del_items_area, temp_items_zr, sizeof(entity_t) * 5) == 0);

    // Check clear records
    size_t total_rec_sz = entity_rec_size(location_entity) + entity_rec_size(amount_entity) + entity_rec_size(city_entity) + entity_rec_size(flag_entity) + entity_rec_size(info_entity);
    void *del_recs_area = my_malloc_array(char, total_rec_sz);
    void *temp_recs_zr = memset(my_malloc_array(char, total_rec_sz), 0, total_rec_sz);
    sect_ext_rd_rec(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, SECTION_SIZE - total_rec_sz), total_rec_sz, del_recs_area);
    assert(memcmp(del_recs_area, temp_recs_zr, total_rec_sz) == 0);

    free(del_items_area);
    free(temp_items_zr);
    free(del_recs_area);
    free(temp_recs_zr);

    json_dtor(info_json);

    entity_dtor(location_entity);
    entity_dtor(amount_entity);
    entity_dtor(city_entity);
    entity_dtor(flag_entity);
    entity_dtor(info_entity);

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
info -> city -> flag -> location -> amount
del city
*/
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

    DO_OR_FAIL(sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, wrt_addr), info_o_entity, info_o_json));

    // Check change ptr dad -> son from info
    assert(info_o_entity->fam_addr.son_ptr == sect_head_get_fileoff(&file->f_extent->header, sizeof(sect_head_entity_t) + 2 * sizeof(entity_t)));

    // Check section 2, 4, 5 empty
    void *del_items_area = my_malloc_array(entity_t, 2);
    void *temp_items_zr = memset(my_malloc_array(entity_t, 2), 0, 2 * sizeof(entity_t));
    sect_ext_rd_rec(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, wrt_addr + 3 * sizeof(entity_t)), sizeof(entity_t) * 2, del_items_area);
    assert(memcmp(del_items_area, temp_items_zr, sizeof(entity_t) * 2) == 0);
    free(del_items_area);
    free(temp_items_zr);

    del_items_area = my_malloc_array(entity_t, 1);
    temp_items_zr = memset(my_malloc_array(entity_t, 1), 0, 1 * sizeof(entity_t));
    sect_ext_rd_rec(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, wrt_addr + sizeof(entity_t)), sizeof(entity_t), del_items_area);
    assert(memcmp(del_items_area, temp_items_zr, sizeof(entity_t)) == 0);
    free(del_items_area);
    free(temp_items_zr);

    // Check ptrs shift
    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t) - entity_ph_size(info_entity) - entity_ph_size(city_entity) - entity_ph_size(flag_entity));
    assert(file->f_extent->header.lst_itm_ptr == sizeof(sect_head_entity_t) + 3 * sizeof(entity_t));
    assert(file->f_extent->header.fst_rec_ptr == SECTION_SIZE - entity_rec_size(info_entity) - entity_rec_size(city_entity) - entity_rec_size(flag_entity));
    assert(file->f_extent->header.next_ptr == 0);
    assert(file->f_extent->header.sect_off == sizeof(file_head_t));

    json_dtor(info_json);
    json_dtor(info_o_json);

    entity_dtor(info_o_entity);
    entity_dtor(location_entity);
    entity_dtor(amount_entity);
    entity_dtor(city_entity);
    entity_dtor(flag_entity);
    entity_dtor(info_entity);

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
info -> city -> flag -> location -> amount
del location
*/
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

    sect_ext_t *sect_ext_aftr_wrt = memcpy(my_malloc(sect_ext_t), file->f_extent, sizeof(sect_ext_t));

    json_t *info_o_json = json_new();
    entity_t *info_o_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, wrt_addr), info_o_entity, info_o_json));

    json_t *city_o_json = json_new();
    entity_t *city_o_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, info_o_entity->fam_addr.son_ptr), city_o_entity, city_o_json));

    DO_OR_FAIL(file_delete(file, city_o_entity->fam_addr.son_ptr));

    DO_OR_FAIL(sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, info_o_entity->fam_addr.son_ptr), city_o_entity, city_o_json));

    // Check change ptr dad -> son from city
    assert(city_o_entity->fam_addr.son_ptr == sect_head_get_fileoff(&file->f_extent->header, sizeof(sect_head_entity_t) + 4 * sizeof(entity_t)));

    // Check section 4 empty
    void *del_items_area = my_malloc(entity_t);
    void *temp_items_zr = memset(my_malloc(entity_t), 0, sizeof(entity_t));
    sect_ext_rd_rec(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, wrt_addr + 3 * sizeof(entity_t)), sizeof(entity_t), del_items_area);
    assert(memcmp(del_items_area, temp_items_zr, sizeof(entity_t)) == 0);
    free(del_items_area);
    free(temp_items_zr);

    // Check ptrs shift
    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == sect_ext_aftr_wrt->header.free_space);
    assert(file->f_extent->header.lst_itm_ptr == sect_ext_aftr_wrt->header.lst_itm_ptr);
    assert(file->f_extent->header.fst_rec_ptr == sect_ext_aftr_wrt->header.fst_rec_ptr);
    assert(file->f_extent->header.next_ptr == sect_ext_aftr_wrt->header.next_ptr);
    assert(file->f_extent->header.sect_off == sect_ext_aftr_wrt->header.sect_off);

    json_dtor(info_json);
    json_dtor(info_o_json);
    json_dtor(city_o_json);

    entity_dtor(info_o_entity);
    entity_dtor(city_o_entity);
    entity_dtor(location_entity);
    entity_dtor(amount_entity);
    entity_dtor(city_entity);
    entity_dtor(flag_entity);
    entity_dtor(info_entity);

    sect_ext_dtor(sect_ext_aftr_wrt);

    file_dtor(file);
    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_file_delete()
{
    JSON_VALUE_INIT(TYPE_OBJECT, obj_json, "json", NULL);
    STR_INIT(json_str, "value");
    JSON_VALUE_INIT(TYPE_STRING, str_json, "json", json_str);
    JSON_VALUE_INIT(TYPE_INT32, int_json, "json", 5);
    JSON_VALUE_INIT(TYPE_FLOAT, float_json, "json", 5.5);
    JSON_VALUE_INIT(TYPE_BOOL, bool_json, "json", true);

    assert(File_DeleteType_ShiftPtrsAndClear(obj_json) == OK);
    assert(File_DeleteType_ShiftPtrsAndClear(str_json) == OK);
    assert(File_DeleteType_ShiftPtrsAndClear(int_json) == OK);
    assert(File_DeleteType_ShiftPtrsAndClear(float_json) == OK);
    assert(File_DeleteType_ShiftPtrsAndClear(bool_json) == OK);
    assert(File_DeleteFirstLevelFromObjectNode_ShiftPtrsAndClearObject() == OK);
    assert(File_DeleteSecondLevelFromObjectNode_ShiftPtrsAndClearObject() == OK);
    assert(File_DeleteThirdLevelFromObjectNode_ShiftPtrsAndClearObject() == OK);

    json_dtor(obj_json);
    json_dtor(str_json);
    json_dtor(int_json);
    json_dtor(float_json);
    json_dtor(bool_json);
}