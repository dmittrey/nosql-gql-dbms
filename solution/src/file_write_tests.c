#include <assert.h>

#include "string.h"

#include "memory/file/file.h"

#include "memory/json/json.h"
#include "physical/json/entity.h"

#include "memory/section/extents.h"

/*
1) Запись строки
2) Запись инта
3) Запись була
4) Запись флота
5) Запись двух объектов подряд в одну секцию
6) Запись составного объекта в пустой файл
7) Запись составного объекта в файл с одной подходящей секцией
8) Запись составного объекта в файл с одной не подходящей секцией
9) Запись трехуровневого объекта
*/

static const char *test_file_name = "test.bin";

static status_t File_WriteString_AddSectionAndWrite()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    STR_INIT(str, "awdawdadw");
    JSON_VALUE_INIT(TYPE_STRING, json, "value", str);
    ENTITY_INIT(entity, json, 0, 0, 0);

    fileoff_t wrt_addr;
    file_write(file, json, 0, &wrt_addr);

    // Проверка создания новой секции
    assert(file->filp == filp);
    assert(file->header.lst_sect_ptr == sizeof(file_head_t) + SECTION_SIZE);
    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t) - entity_ph_size(entity));
    assert(file->f_extent->header.fst_rec_ptr == SECTION_SIZE - entity_rec_size(entity));
    assert(file->f_extent->header.lst_itm_ptr == sizeof(sect_head_entity_t) + entity_itm_size(entity));
    assert(file->f_extent->header.next_ptr == 0);
    assert(file->f_extent->header.sect_off == sizeof(file_head_t));

    // Проверка записи в секцию валидного объекта
    entity_t *o_entity = entity_new();
    json_t *o_json = json_new();
    sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, wrt_addr), o_entity, o_json);

    assert(o_entity->key_ptr == SECTION_SIZE - entity->key_size);
    assert(o_entity->key_size == entity->key_size);
    assert(o_entity->val_ptr == SECTION_SIZE - entity->key_size - entity->val_size);
    assert(o_entity->val_size == entity->val_size);
    assert(o_entity->rec_size == entity->key_size + entity->val_size);
    assert(o_entity->type == entity->type);
    assert(o_entity->fam_addr.dad_ptr == entity->fam_addr.dad_ptr);
    assert(o_entity->fam_addr.bro_ptr == entity->fam_addr.bro_ptr);
    assert(o_entity->fam_addr.son_ptr == entity->fam_addr.son_ptr);

    assert(o_json->key->cnt == json->key->cnt);
    assert(strncmp(o_json->key->val, json->key->val, o_json->key->cnt) == 0);
    assert(o_json->value.string_val->cnt == json->value.string_val->cnt);
    assert(strncmp(o_json->value.string_val->val, json->value.string_val->val, o_json->value.string_val->cnt) == 0);

    json_dtor(json);
    json_dtor(o_json);

    entity_dtor(entity);
    entity_dtor(o_entity);

    file_dtor(file);
    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t File_WriteInt32_AddSectionAndWrite()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    JSON_VALUE_INIT(TYPE_INT32, json, "value", 5);
    ENTITY_INIT(entity, json, 0, 0, 0);

    fileoff_t wrt_addr;
    file_write(file, json, 0, &wrt_addr);

    // Проверка создания новой секции
    assert(file->filp == filp);
    assert(file->header.lst_sect_ptr == sizeof(file_head_t) + SECTION_SIZE);
    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t) - entity_ph_size(entity));
    assert(file->f_extent->header.fst_rec_ptr == SECTION_SIZE - entity_rec_size(entity));
    assert(file->f_extent->header.lst_itm_ptr == sizeof(sect_head_entity_t) + entity_itm_size(entity));
    assert(file->f_extent->header.next_ptr == 0);
    assert(file->f_extent->header.sect_off == sizeof(file_head_t));

    // Проверка записи в секцию валидного объекта
    entity_t *o_entity = entity_new();
    json_t *o_json = json_new();
    sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, wrt_addr), o_entity, o_json);

    assert(o_entity->key_ptr == SECTION_SIZE - entity->key_size);
    assert(o_entity->key_size == entity->key_size);
    assert(o_entity->val_ptr == SECTION_SIZE - entity->key_size - entity->val_size);
    assert(o_entity->val_size == entity->val_size);
    assert(o_entity->rec_size == entity->key_size + entity->val_size);
    assert(o_entity->type == entity->type);
    assert(o_entity->fam_addr.dad_ptr == entity->fam_addr.dad_ptr);
    assert(o_entity->fam_addr.bro_ptr == entity->fam_addr.bro_ptr);
    assert(o_entity->fam_addr.son_ptr == entity->fam_addr.son_ptr);

    json_dtor(json);
    json_dtor(o_json);

    entity_dtor(entity);
    entity_dtor(o_entity);

    file_dtor(file);
    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t File_WriteBool_AddSectionAndWrite()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    JSON_VALUE_INIT(TYPE_BOOL, json, "value", true);
    ENTITY_INIT(entity, json, 0, 0, 0);

    fileoff_t wrt_addr;
    file_write(file, json, 0, &wrt_addr);

    // Проверка создания новой секции
    assert(file->filp == filp);
    assert(file->header.lst_sect_ptr == sizeof(file_head_t) + SECTION_SIZE);
    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t) - entity_ph_size(entity));
    assert(file->f_extent->header.fst_rec_ptr == SECTION_SIZE - entity_rec_size(entity));
    assert(file->f_extent->header.lst_itm_ptr == sizeof(sect_head_entity_t) + entity_itm_size(entity));
    assert(file->f_extent->header.next_ptr == 0);
    assert(file->f_extent->header.sect_off == sizeof(file_head_t));

    // Проверка записи в секцию валидного объекта
    entity_t *o_entity = entity_new();
    json_t *o_json = json_new();
    sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, wrt_addr), o_entity, o_json);

    assert(o_entity->key_ptr == SECTION_SIZE - entity->key_size);
    assert(o_entity->key_size == entity->key_size);
    assert(o_entity->val_ptr == SECTION_SIZE - entity->key_size - entity->val_size);
    assert(o_entity->val_size == entity->val_size);
    assert(o_entity->rec_size == entity->key_size + entity->val_size);
    assert(o_entity->type == entity->type);
    assert(o_entity->fam_addr.dad_ptr == entity->fam_addr.dad_ptr);
    assert(o_entity->fam_addr.bro_ptr == entity->fam_addr.bro_ptr);
    assert(o_entity->fam_addr.son_ptr == entity->fam_addr.son_ptr);

    json_dtor(json);
    json_dtor(o_json);

    entity_dtor(entity);
    entity_dtor(o_entity);

    file_dtor(file);
    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t File_WriteFloat_AddSectionAndWrite()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    JSON_VALUE_INIT(TYPE_FLOAT, json, "value", 5.5);
    ENTITY_INIT(entity, json, 0, 0, 0);

    fileoff_t wrt_addr;
    file_write(file, json, 0, &wrt_addr);

    // Проверка создания новой секции
    assert(file->filp == filp);
    assert(file->header.lst_sect_ptr == sizeof(file_head_t) + SECTION_SIZE);
    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t) - entity_ph_size(entity));
    assert(file->f_extent->header.fst_rec_ptr == SECTION_SIZE - entity_rec_size(entity));
    assert(file->f_extent->header.lst_itm_ptr == sizeof(sect_head_entity_t) + entity_itm_size(entity));
    assert(file->f_extent->header.next_ptr == 0);
    assert(file->f_extent->header.sect_off == sizeof(file_head_t));

    // Проверка записи в секцию валидного объекта
    entity_t *o_entity = entity_new();
    json_t *o_json = json_new();
    sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, wrt_addr), o_entity, o_json);

    assert(o_entity->key_ptr == SECTION_SIZE - entity->key_size);
    assert(o_entity->key_size == entity->key_size);
    assert(o_entity->val_ptr == SECTION_SIZE - entity->key_size - entity->val_size);
    assert(o_entity->val_size == entity->val_size);
    assert(o_entity->rec_size == entity->key_size + entity->val_size);
    assert(o_entity->type == entity->type);
    assert(o_entity->fam_addr.dad_ptr == entity->fam_addr.dad_ptr);
    assert(o_entity->fam_addr.bro_ptr == entity->fam_addr.bro_ptr);
    assert(o_entity->fam_addr.son_ptr == entity->fam_addr.son_ptr);

    json_dtor(json);
    json_dtor(o_json);

    entity_dtor(entity);
    entity_dtor(o_entity);

    file_dtor(file);
    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t File_WriteTwoStrings_AddSectionAndWriteTwoObjectsInOneSect()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    STR_INIT(f_str, "first");
    JSON_VALUE_INIT(TYPE_STRING, f_json, "value", f_str);
    ENTITY_INIT(f_entity, f_json, 0, 0, 0);

    STR_INIT(s_str, "second");
    JSON_VALUE_INIT(TYPE_STRING, s_json, "value", s_str);
    ENTITY_INIT(s_entity, s_json, 0, 0, 0);

    fileoff_t f_wrt_addr;
    file_write(file, f_json, 0, &f_wrt_addr);
    fileoff_t s_wrt_addr;
    file_write(file, s_json, 0, &s_wrt_addr);

    // Проверка создания новой секции
    assert(file->filp == filp);
    assert(file->header.lst_sect_ptr == sizeof(file_head_t) + SECTION_SIZE);
    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t) - entity_ph_size(f_entity) - entity_ph_size(s_entity));
    assert(file->f_extent->header.fst_rec_ptr == SECTION_SIZE - entity_rec_size(f_entity) - entity_rec_size(s_entity));
    assert(file->f_extent->header.lst_itm_ptr == sizeof(sect_head_entity_t) + entity_itm_size(f_entity) + entity_itm_size(s_entity));
    assert(file->f_extent->header.next_ptr == 0);
    assert(file->f_extent->header.sect_off == sizeof(file_head_t));

    // Проверка записи в секцию валидного объекта
    entity_t *f_o_entity = entity_new();
    json_t *f_o_json = json_new();
    sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, f_wrt_addr), f_o_entity, f_o_json);

    entity_t *s_o_entity = entity_new();
    json_t *s_o_json = json_new();
    sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, s_wrt_addr), s_o_entity, s_o_json);

    assert(f_o_entity->key_ptr == SECTION_SIZE - f_entity->key_size);
    assert(f_o_entity->key_size == f_entity->key_size);
    assert(f_o_entity->val_ptr == SECTION_SIZE - f_entity->key_size - f_entity->val_size);
    assert(f_o_entity->val_size == f_entity->val_size);
    assert(f_o_entity->rec_size == f_entity->key_size + f_entity->val_size);
    assert(f_o_entity->type == f_entity->type);
    assert(f_o_entity->fam_addr.dad_ptr == f_entity->fam_addr.dad_ptr);
    assert(f_o_entity->fam_addr.bro_ptr == f_entity->fam_addr.bro_ptr);
    assert(f_o_entity->fam_addr.son_ptr == f_entity->fam_addr.son_ptr);

    assert(s_o_entity->key_ptr == SECTION_SIZE - f_entity->key_size - f_entity->val_size - s_entity->key_size);
    assert(s_o_entity->key_size == s_entity->key_size);
    assert(s_o_entity->val_ptr == SECTION_SIZE - f_entity->key_size - f_entity->val_size - s_entity->key_size - s_entity->val_size);
    assert(s_o_entity->val_size == s_entity->val_size);
    assert(s_o_entity->rec_size == s_entity->key_size + s_entity->val_size);
    assert(s_o_entity->type == s_entity->type);
    assert(s_o_entity->fam_addr.dad_ptr == s_entity->fam_addr.dad_ptr);
    assert(s_o_entity->fam_addr.bro_ptr == s_entity->fam_addr.bro_ptr);
    assert(s_o_entity->fam_addr.son_ptr == s_entity->fam_addr.son_ptr);

    json_dtor(f_json);
    json_dtor(s_json);
    json_dtor(f_o_json);
    json_dtor(s_o_json);

    entity_dtor(f_entity);
    entity_dtor(s_entity);
    entity_dtor(f_o_entity);
    entity_dtor(s_o_entity);

    file_dtor(file);
    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

/*
{
   "obj" : {
        "son" : "value"
    },
    "bro" : "example"
}
*/
static status_t File_WriteObject_AddSectionAndWriteComponents()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    STR_INIT(son_str, "value");
    JSON_VALUE_INIT(TYPE_STRING, son_json, "son", son_str);
    ENTITY_INIT(son_entity, son_json, 0, 0, 0);

    STR_INIT(bro_str, "example");
    JSON_VALUE_INIT(TYPE_STRING, bro_json, "bro", bro_str);
    ENTITY_INIT(bro_entity, bro_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, obj_json, "obj", NULL);
    ENTITY_INIT(obj_entity, obj_json, 0, 0, 0);

    json_add_bro(obj_json, bro_json);
    json_add_son(obj_json, son_json);

    fileoff_t obj_wrt_addr;
    file_write(file, obj_json, 0, &obj_wrt_addr);

    assert(file->filp == filp);
    assert(file->header.lst_sect_ptr == sizeof(file_head_t) + SECTION_SIZE);

    assert(file->filp == filp);
    assert(file->header.lst_sect_ptr == sizeof(file_head_t) + SECTION_SIZE);
    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t) - entity_ph_size(obj_entity) - entity_ph_size(bro_entity) - entity_ph_size(son_entity));
    assert(file->f_extent->header.fst_rec_ptr == SECTION_SIZE - entity_rec_size(obj_entity) - entity_rec_size(bro_entity) - entity_rec_size(son_entity));
    assert(file->f_extent->header.lst_itm_ptr == sizeof(sect_head_entity_t) + entity_itm_size(obj_entity) + entity_itm_size(bro_entity) + entity_itm_size(son_entity));
    assert(file->f_extent->header.next_ptr == 0);
    assert(file->f_extent->header.sect_off == sizeof(file_head_t));

    entity_t *obj_o_entity = entity_new();
    json_t *obj_o_json = json_new();
    sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, obj_wrt_addr), obj_o_entity, obj_o_json);

    entity_t *bro_o_entity = entity_new();
    json_t *bro_o_json = json_new();
    sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, obj_o_entity->fam_addr.bro_ptr), bro_o_entity, bro_o_json);

    entity_t *son_o_entity = entity_new();
    json_t *son_o_json = json_new();
    sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, obj_o_entity->fam_addr.son_ptr), son_o_entity, son_o_json);

    assert(obj_o_entity->key_ptr == SECTION_SIZE - obj_entity->key_size);
    assert(obj_o_entity->key_size == obj_entity->key_size);
    assert(obj_o_entity->val_ptr == SECTION_SIZE - obj_entity->key_size);
    assert(obj_o_entity->val_size == 0);
    assert(obj_o_entity->rec_size == obj_entity->key_size);
    assert(obj_o_entity->type == obj_entity->type);
    assert(obj_o_entity->fam_addr.dad_ptr == 0);
    assert(obj_o_entity->fam_addr.bro_ptr == sizeof(file_head_t) + sizeof(sect_head_entity_t) + sizeof(entity_t));
    assert(obj_o_entity->fam_addr.son_ptr == sizeof(file_head_t) + sizeof(sect_head_entity_t) + 2 * sizeof(entity_t));

    assert(bro_o_entity->key_ptr == SECTION_SIZE - entity_rec_size(obj_entity) - bro_entity->key_size);
    assert(bro_o_entity->key_size == bro_entity->key_size);
    assert(bro_o_entity->val_ptr == SECTION_SIZE - entity_rec_size(obj_entity) - entity_rec_size(bro_entity));
    assert(bro_o_entity->val_size == bro_entity->val_size);
    assert(bro_o_entity->rec_size == entity_rec_size(bro_entity));
    assert(bro_o_entity->type == bro_entity->type);
    assert(bro_o_entity->fam_addr.dad_ptr == 0);
    assert(bro_o_entity->fam_addr.bro_ptr == 0);
    assert(bro_o_entity->fam_addr.son_ptr == 0);

    assert(son_o_entity->key_ptr == SECTION_SIZE - entity_rec_size(obj_entity) - entity_rec_size(bro_entity) - son_entity->key_size);
    assert(son_o_entity->key_size == son_entity->key_size);
    assert(son_o_entity->val_ptr == SECTION_SIZE - entity_rec_size(obj_entity) - entity_rec_size(bro_entity) - entity_rec_size(son_entity));
    assert(son_o_entity->val_size == son_entity->val_size);
    assert(son_o_entity->rec_size == entity_rec_size(son_entity));
    assert(son_o_entity->type == son_entity->type);
    assert(son_o_entity->fam_addr.dad_ptr == obj_wrt_addr);
    assert(son_o_entity->fam_addr.bro_ptr == 0);
    assert(son_o_entity->fam_addr.son_ptr == 0);

    json_dtor(bro_json);
    json_dtor(obj_json);
    json_dtor(obj_o_json);
    json_dtor(bro_o_json);
    json_dtor(son_o_json);

    entity_dtor(obj_entity);
    entity_dtor(bro_entity);
    entity_dtor(son_entity);
    entity_dtor(obj_o_entity);
    entity_dtor(bro_o_entity);
    entity_dtor(son_o_entity);

    file_dtor(file);
    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t File_WriteObjectInFileWithExistExtentsion_WriteComponents()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    sect_ext_t *extension = sect_ext_new();
    file_add_sect_ext(file, extension);

    STR_INIT(f_str, "first");
    JSON_VALUE_INIT(TYPE_STRING, f_json, "value", f_str);
    ENTITY_INIT(f_entity, f_json, 0, 0, 0);

    STR_INIT(s_str, "second");
    JSON_VALUE_INIT(TYPE_STRING, s_json, "value", s_str);
    ENTITY_INIT(s_entity, s_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, obj_json, "value", NULL);
    ENTITY_INIT(obj_entity, obj_json, 0, 0, 0);

    json_add_bro(obj_json, f_json);
    json_add_son(obj_json, s_json);

    fileoff_t obj_wrt_addr;
    file_write(file, obj_json, 0, &obj_wrt_addr);

    assert(file->filp == filp);
    assert(file->header.lst_sect_ptr == sizeof(file_head_t) + SECTION_SIZE);

    assert(file->f_extent == extension);
    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t) - entity_ph_size(f_entity) - entity_ph_size(s_entity) - entity_ph_size(obj_entity));
    assert(file->f_extent->header.fst_rec_ptr == SECTION_SIZE - entity_rec_size(f_entity) - entity_rec_size(s_entity) - entity_rec_size(obj_entity));
    assert(file->f_extent->header.lst_itm_ptr == sizeof(sect_head_entity_t) + entity_itm_size(f_entity) + entity_itm_size(s_entity) + entity_itm_size(obj_entity));
    assert(file->f_extent->header.next_ptr == 0);
    assert(file->f_extent->header.sect_off == sizeof(file_head_t));
    assert(file->f_extent->next == NULL);

    json_dtor(f_json);
    json_dtor(obj_json);

    entity_dtor(f_entity);
    entity_dtor(s_entity);
    entity_dtor(obj_entity);

    file_dtor(file);
    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

/*
{
   "obj" : {
        "son" : "value"
    },
    "bro" : "example"
}

In cur section can fill only obj and bro
Son in next "autogenerated" section
*/
static status_t File_WriteObjectInFileWithExistFilledExtentsion_AddSectionWriteComponents()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    sect_ext_t *extension = sect_ext_new();
    file_add_sect_ext(file, extension);
    sect_head_shift_lst_itm_ptr((sect_head_t *)extension, 8000);

    STR_INIT(son_str, "value");
    JSON_VALUE_INIT(TYPE_STRING, son_json, "son", son_str);
    ENTITY_INIT(son_entity, son_json, 0, 0, 0);

    STR_INIT(bro_str, "example");
    JSON_VALUE_INIT(TYPE_STRING, bro_json, "bro", bro_str);
    ENTITY_INIT(bro_entity, bro_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, obj_json, "obj", NULL);
    ENTITY_INIT(obj_entity, obj_json, 0, 0, 0);

    json_add_bro(obj_json, bro_json);
    json_add_son(obj_json, son_json);

    fileoff_t obj_wrt_addr;
    file_write(file, obj_json, 0, &obj_wrt_addr);

    assert(file->filp == filp);
    assert(file->header.lst_sect_ptr == sizeof(file_head_t) + 2 * SECTION_SIZE);

    size_t a = sizeof(sect_head_entity_t);
    size_t b = entity_ph_size(obj_entity);
    size_t c = entity_ph_size(bro_entity);
    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == SECTION_SIZE - 8000 - sizeof(sect_head_entity_t) - entity_ph_size(obj_entity) - entity_ph_size(bro_entity));
    assert(file->f_extent->header.fst_rec_ptr == SECTION_SIZE - entity_rec_size(obj_entity) - entity_rec_size(bro_entity));
    assert(file->f_extent->header.lst_itm_ptr == sizeof(sect_head_entity_t) + 8000 + entity_itm_size(obj_entity) + entity_itm_size(bro_entity));
    assert(file->f_extent->header.next_ptr == sizeof(file_head_t) + SECTION_SIZE);
    assert(file->f_extent->header.sect_off == sizeof(file_head_t));

    assert(file->f_extent->next->header.filp == filp);
    assert(file->f_extent->next->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t) - entity_ph_size(son_entity));
    assert(file->f_extent->next->header.fst_rec_ptr == SECTION_SIZE - entity_rec_size(son_entity));
    assert(file->f_extent->next->header.lst_itm_ptr == sizeof(sect_head_entity_t) + entity_itm_size(son_entity));
    assert(file->f_extent->next->header.next_ptr == 0);
    assert(file->f_extent->next->header.sect_off == SECTION_SIZE + sizeof(file_head_t));

    entity_t *obj_o_entity = entity_new();
    json_t *obj_o_json = json_new();
    sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, obj_wrt_addr), obj_o_entity, obj_o_json);

    entity_t *bro_o_entity = entity_new();
    json_t *bro_o_json = json_new();
    sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, obj_o_entity->fam_addr.bro_ptr), bro_o_entity, bro_o_json);

    entity_t *son_o_entity = entity_new();
    json_t *son_o_json = json_new();
    sect_ext_read(file->f_extent->next, sect_head_get_sectoff(&file->f_extent->next->header, obj_o_entity->fam_addr.son_ptr), son_o_entity, son_o_json);

    assert(obj_o_entity->key_ptr == SECTION_SIZE - obj_entity->key_size);
    assert(obj_o_entity->key_size == obj_entity->key_size);
    assert(obj_o_entity->val_ptr == SECTION_SIZE - obj_entity->key_size);
    assert(obj_o_entity->val_size == 0);
    assert(obj_o_entity->rec_size == obj_entity->key_size);
    assert(obj_o_entity->type == obj_entity->type);
    assert(obj_o_entity->fam_addr.dad_ptr == 0);
    assert(obj_o_entity->fam_addr.bro_ptr == sizeof(file_head_t) + sizeof(sect_head_entity_t) + 8000 + sizeof(entity_t));
    assert(obj_o_entity->fam_addr.son_ptr == sizeof(file_head_t) + SECTION_SIZE + sizeof(sect_head_entity_t));

    assert(obj_o_json->key->cnt == obj_json->key->cnt);
    assert(strncmp(obj_o_json->key->val, obj_json->key->val, obj_o_json->key->cnt) == 0);

    assert(bro_o_entity->key_ptr == SECTION_SIZE - entity_rec_size(obj_entity) - bro_entity->key_size);
    assert(bro_o_entity->key_size == bro_entity->key_size);
    assert(bro_o_entity->val_ptr == SECTION_SIZE - entity_rec_size(obj_entity) - entity_rec_size(bro_entity));
    assert(bro_o_entity->val_size == bro_entity->val_size);
    assert(bro_o_entity->rec_size == entity_rec_size(bro_entity));
    assert(bro_o_entity->type == bro_entity->type);
    assert(bro_o_entity->fam_addr.dad_ptr == 0);
    assert(bro_o_entity->fam_addr.bro_ptr == 0);
    assert(bro_o_entity->fam_addr.son_ptr == 0);

    assert(bro_o_json->key->cnt == bro_json->key->cnt);
    assert(strncmp(bro_o_json->key->val, bro_json->key->val, bro_o_json->key->cnt) == 0);
    assert(bro_o_json->value.string_val->cnt == bro_json->value.string_val->cnt);
    assert(strncmp(bro_o_json->value.string_val->val, bro_json->value.string_val->val, bro_o_json->value.string_val->cnt) == 0);

    assert(son_o_entity->key_ptr == SECTION_SIZE - son_entity->key_size);
    assert(son_o_entity->key_size == son_entity->key_size);
    assert(son_o_entity->val_ptr == SECTION_SIZE - entity_rec_size(son_entity));
    assert(son_o_entity->val_size == son_entity->val_size);
    assert(son_o_entity->rec_size == entity_rec_size(son_entity));
    assert(son_o_entity->type == son_entity->type);
    assert(son_o_entity->fam_addr.dad_ptr == obj_wrt_addr);
    assert(son_o_entity->fam_addr.bro_ptr == 0);
    assert(son_o_entity->fam_addr.son_ptr == 0);

    assert(son_o_json->key->cnt == son_json->key->cnt);
    assert(strncmp(son_o_json->key->val, son_json->key->val, son_o_json->key->cnt) == 0);
    assert(son_o_json->value.string_val->cnt == son_json->value.string_val->cnt);
    assert(strncmp(son_o_json->value.string_val->val, son_json->value.string_val->val, son_o_json->value.string_val->cnt) == 0);

    json_dtor(bro_json);
    json_dtor(obj_json);
    json_dtor(obj_o_json);
    json_dtor(bro_o_json);
    json_dtor(son_o_json);

    entity_dtor(obj_entity);
    entity_dtor(bro_entity);
    entity_dtor(son_entity);
    entity_dtor(obj_o_entity);
    entity_dtor(bro_o_entity);
    entity_dtor(son_o_entity);

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
/*
1) info
2) city
3) flag
4) location
5) amount
*/
static status_t File_WriteThreeLevelsObject_AddSectionWriteComponents()
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

    json_add_son(city_json, location_json);
    json_add_son(city_json, amount_json);

    JSON_VALUE_INIT(TYPE_BOOL, flag_json, "flag", true);
    ENTITY_INIT(flag_entity, flag_json, 0, 0, 0);

    JSON_VALUE_INIT(TYPE_OBJECT, info_json, "info", NULL);
    ENTITY_INIT(info_entity, info_json, 0, 0, 0);

    json_add_son(info_json, city_json);
    json_add_son(info_json, flag_json);

    fileoff_t info_wrt_addr;
    file_write(file, info_json, 0, &info_wrt_addr);

    assert(file->filp == filp);
    assert(file->header.lst_sect_ptr == sizeof(file_head_t) + SECTION_SIZE);

    assert(file->f_extent != NULL);
    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t) - entity_ph_size(flag_entity) - entity_ph_size(amount_entity) - entity_ph_size(location_entity) - entity_ph_size(city_entity) - entity_ph_size(info_entity));
    assert(file->f_extent->header.fst_rec_ptr == SECTION_SIZE - entity_rec_size(flag_entity) - entity_rec_size(amount_entity) - entity_rec_size(location_entity) - entity_rec_size(city_entity) - entity_rec_size(info_entity));
    assert(file->f_extent->header.lst_itm_ptr == sizeof(sect_head_entity_t) + entity_itm_size(flag_entity) + entity_itm_size(amount_entity) + entity_itm_size(location_entity) + entity_itm_size(city_entity) + entity_itm_size(info_entity));
    assert(file->f_extent->header.next_ptr == 0);
    assert(file->f_extent->header.sect_off == sizeof(file_head_t));

    entity_t *info_o_entity = entity_new();
    json_t *info_o_json = json_new();
    sect_ext_read(file->f_extent, sizeof(sect_head_entity_t), info_o_entity, info_o_json);

    assert(info_o_entity->key_ptr == SECTION_SIZE - info_entity->key_size);
    assert(info_o_entity->key_size == info_entity->key_size);
    assert(info_o_entity->val_ptr == SECTION_SIZE - info_entity->key_size);
    assert(info_o_entity->val_size == 0);
    assert(info_o_entity->rec_size == entity_rec_size(info_entity));
    assert(info_o_entity->type == info_entity->type);
    assert(info_o_entity->fam_addr.dad_ptr == 0);
    assert(info_o_entity->fam_addr.bro_ptr == 0);
    assert(info_o_entity->fam_addr.son_ptr == sizeof(file_head_t) + sizeof(sect_head_entity_t) + sizeof(entity_t));

    entity_t *city_o_entity = entity_new();
    json_t *city_o_json = json_new();
    sect_ext_read(file->f_extent, sizeof(sect_head_entity_t) + sizeof(entity_t), city_o_entity, city_o_json);

    assert(city_o_entity->key_ptr == info_o_entity->key_ptr - city_entity->key_size);
    assert(city_o_entity->key_size == city_entity->key_size);
    assert(city_o_entity->val_ptr == info_o_entity->key_ptr - city_entity->key_size);
    assert(city_o_entity->val_size == city_entity->val_size);
    assert(city_o_entity->rec_size == city_entity->key_size + city_entity->val_size);
    assert(city_o_entity->type == city_entity->type);
    assert(city_o_entity->fam_addr.dad_ptr == sizeof(file_head_t) + sizeof(sect_head_entity_t));
    assert(city_o_entity->fam_addr.bro_ptr == sizeof(file_head_t) + sizeof(sect_head_entity_t) + 2 * sizeof(entity_t));
    assert(city_o_entity->fam_addr.son_ptr == sect_head_get_fileoff(&file->f_extent->header, sizeof(sect_head_entity_t) + 3 * sizeof(entity_t)));

    entity_t *flag_o_entity = entity_new();
    json_t *flag_o_json = json_new();
    sect_ext_read(file->f_extent, sizeof(sect_head_entity_t) + 2 * sizeof(entity_t), flag_o_entity, flag_o_json);

    assert(flag_o_entity->key_ptr == city_o_entity->key_ptr - flag_entity->key_size);
    assert(flag_o_entity->key_size == flag_entity->key_size);
    assert(flag_o_entity->val_ptr == city_o_entity->key_ptr - flag_entity->key_size - flag_entity->val_size);
    assert(flag_o_entity->val_size == flag_entity->val_size);
    assert(flag_o_entity->rec_size == flag_entity->key_size + flag_entity->val_size);
    assert(flag_o_entity->type == flag_entity->type);
    assert(flag_o_entity->fam_addr.dad_ptr == sect_head_get_fileoff(&file->f_extent->header, sizeof(sect_head_entity_t)));
    assert(flag_o_entity->fam_addr.bro_ptr == 0);
    assert(flag_o_entity->fam_addr.son_ptr == 0);

    entity_t *location_o_entity = entity_new();
    json_t *location_o_json = json_new();
    sect_ext_read(file->f_extent, sizeof(sect_head_entity_t) + 3 * sizeof(entity_t), location_o_entity, location_o_json);

    assert(location_o_entity->key_ptr == flag_o_entity->val_ptr - location_entity->key_size);
    assert(location_o_entity->key_size == location_entity->key_size);
    assert(location_o_entity->val_ptr == flag_o_entity->val_ptr - location_entity->key_size - location_entity->val_size);
    assert(location_o_entity->val_size == location_entity->val_size);
    assert(location_o_entity->rec_size == location_entity->key_size + location_entity->val_size);
    assert(location_o_entity->type == location_entity->type);
    assert(location_o_entity->fam_addr.dad_ptr == sect_head_get_fileoff(&file->f_extent->header, sizeof(sect_head_entity_t) + sizeof(entity_t)));
    assert(location_o_entity->fam_addr.bro_ptr == sect_head_get_fileoff(&file->f_extent->header, sizeof(sect_head_entity_t) + 4 * sizeof(entity_t)));
    assert(location_o_entity->fam_addr.son_ptr == 0);

    entity_t *amount_o_entity = entity_new();
    json_t *amount_o_json = json_new();
    sect_ext_read(file->f_extent, sizeof(sect_head_entity_t) + 4 * sizeof(entity_t), amount_o_entity, amount_o_json);

    assert(amount_o_entity->key_ptr == location_o_entity->val_ptr - amount_entity->key_size);
    assert(amount_o_entity->key_size == amount_entity->key_size);
    assert(amount_o_entity->val_ptr == location_o_entity->val_ptr - entity_rec_size(amount_entity));
    assert(amount_o_entity->val_size == amount_entity->val_size);
    assert(amount_o_entity->rec_size == amount_entity->key_size + amount_entity->val_size);
    assert(amount_o_entity->type == amount_entity->type);
    assert(amount_o_entity->fam_addr.dad_ptr == sect_head_get_fileoff(&file->f_extent->header, sizeof(sect_head_entity_t) + sizeof(entity_t)));
    assert(amount_o_entity->fam_addr.bro_ptr == 0);
    assert(amount_o_entity->fam_addr.son_ptr == 0);

    json_dtor(info_json);

    json_dtor(location_o_json);
    json_dtor(amount_o_json);
    json_dtor(city_o_json);
    json_dtor(flag_o_json);
    json_dtor(info_o_json);

    entity_dtor(location_entity);
    entity_dtor(amount_entity);
    entity_dtor(city_entity);
    entity_dtor(flag_entity);
    entity_dtor(info_entity);
    entity_dtor(location_o_entity);
    entity_dtor(amount_o_entity);
    entity_dtor(city_o_entity);
    entity_dtor(flag_o_entity);
    entity_dtor(info_o_entity);

    file_dtor(file);
    fclose(filp);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_file_write()
{
    assert(File_WriteString_AddSectionAndWrite() == OK);
    assert(File_WriteInt32_AddSectionAndWrite() == OK);
    assert(File_WriteBool_AddSectionAndWrite() == OK);
    assert(File_WriteFloat_AddSectionAndWrite() == OK);
    assert(File_WriteTwoStrings_AddSectionAndWriteTwoObjectsInOneSect() == OK);
    assert(File_WriteObject_AddSectionAndWriteComponents() == OK);
    assert(File_WriteObjectInFileWithExistExtentsion_WriteComponents() == OK);
    assert(File_WriteObjectInFileWithExistFilledExtentsion_AddSectionWriteComponents() == OK);
    assert(File_WriteThreeLevelsObject_AddSectionWriteComponents() == OK);
}