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

static status_t File_WriteObject_AddSectionAndWriteComponents()
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

    JSON_VALUE_INIT(TYPE_OBJECT, obj_json, "value", NULL);
    ENTITY_INIT(obj_entity, obj_json, 0, 0, 0);

    json_add_bro(obj_json, f_json);
    json_add_son(obj_json, s_json);

    fileoff_t obj_wrt_addr;
    file_write(file, obj_json, 0, &obj_wrt_addr);

    assert(file->filp == filp);
    assert(file->header.lst_sect_ptr == sizeof(file_head_t) + SECTION_SIZE);
    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t) - entity_ph_size(f_entity) - entity_ph_size(s_entity) - entity_ph_size(obj_entity));
    assert(file->f_extent->header.fst_rec_ptr == SECTION_SIZE - entity_rec_size(f_entity) - entity_rec_size(s_entity) - entity_rec_size(obj_entity));
    assert(file->f_extent->header.lst_itm_ptr == sizeof(sect_head_entity_t) + entity_itm_size(f_entity) + entity_itm_size(s_entity) + entity_itm_size(obj_entity));
    assert(file->f_extent->header.next_ptr == 0);
    assert(file->f_extent->header.sect_off == sizeof(file_head_t));

    entity_t *obj_o_entity = entity_new();
    json_t *obj_o_json = json_new();
    sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, obj_wrt_addr), obj_o_entity, obj_o_json);

    entity_t *f_o_entity = entity_new();
    json_t *f_o_json = json_new();
    sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, obj_o_entity->fam_addr.bro_ptr), f_o_entity, f_o_json);

    entity_t *s_o_entity = entity_new();
    json_t *s_o_json = json_new();
    sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, obj_o_entity->fam_addr.son_ptr), s_o_entity, s_o_json);

    assert(f_o_entity->key_ptr == SECTION_SIZE - f_entity->key_size);
    assert(f_o_entity->key_size == f_entity->key_size);
    assert(f_o_entity->val_ptr == SECTION_SIZE - f_entity->key_size - f_entity->val_size);
    assert(f_o_entity->val_size == f_entity->val_size);
    assert(f_o_entity->rec_size == f_entity->key_size + f_entity->val_size);
    assert(f_o_entity->type == f_entity->type);
    assert(f_o_entity->fam_addr.dad_ptr == 0);
    assert(f_o_entity->fam_addr.bro_ptr == 0);
    assert(f_o_entity->fam_addr.son_ptr == 0);

    assert(s_o_entity->key_ptr == SECTION_SIZE - f_entity->key_size - f_entity->val_size - s_entity->key_size);
    assert(s_o_entity->key_size == s_entity->key_size);
    assert(s_o_entity->val_ptr == SECTION_SIZE - f_entity->key_size - f_entity->val_size - s_entity->key_size - s_entity->val_size);
    assert(s_o_entity->val_size == s_entity->val_size);
    assert(s_o_entity->rec_size == s_entity->key_size + s_entity->val_size);
    assert(s_o_entity->type == s_entity->type);
    assert(s_o_entity->fam_addr.dad_ptr == obj_wrt_addr);
    assert(s_o_entity->fam_addr.bro_ptr == 0);
    assert(s_o_entity->fam_addr.son_ptr == 0);

    assert(obj_o_entity->key_ptr == SECTION_SIZE - f_entity->key_size - f_entity->val_size - s_entity->key_size - s_entity->val_size - obj_entity->key_size);
    assert(obj_o_entity->key_size == obj_entity->key_size);
    assert(obj_o_entity->val_ptr == 0);
    assert(obj_o_entity->val_size == 0);
    assert(obj_o_entity->rec_size == obj_entity->key_size + obj_entity->val_size);
    assert(obj_o_entity->type == obj_entity->type);
    assert(obj_o_entity->fam_addr.dad_ptr == 0);

    json_dtor(f_json);
    json_dtor(obj_json);
    json_dtor(f_o_json);
    json_dtor(s_o_json);
    json_dtor(obj_o_json);

    entity_dtor(f_entity);
    entity_dtor(s_entity);
    entity_dtor(obj_entity);
    entity_dtor(f_o_entity);
    entity_dtor(s_o_entity);
    entity_dtor(obj_o_entity);

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

static status_t File_WriteObjectInFileWithExistFilledExtentsion_AddSectionWriteComponents()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    sect_ext_t *extension = sect_ext_new();
    file_add_sect_ext(file, extension);
    sect_head_shift_lst_itm_ptr((sect_head_t *)extension, 8000);

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
    assert(file->header.lst_sect_ptr == sizeof(file_head_t) + 2 * SECTION_SIZE);

    assert(file->f_extent == extension);
    assert(file->f_extent->header.filp == filp);
    assert(file->f_extent->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t) - 8000 - entity_ph_size(f_entity) - entity_ph_size(obj_entity));
    assert(file->f_extent->header.fst_rec_ptr == SECTION_SIZE - entity_rec_size(f_entity) - entity_rec_size(obj_entity));
    assert(file->f_extent->header.lst_itm_ptr == sizeof(sect_head_entity_t) + 8000 + entity_itm_size(f_entity) + entity_itm_size(obj_entity));
    assert(file->f_extent->header.next_ptr == sizeof(file_head_t) + SECTION_SIZE);
    assert(file->f_extent->header.sect_off == sizeof(file_head_t));

    assert(file->f_extent->next->header.filp == filp);
    assert(file->f_extent->next->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t) - entity_ph_size(s_entity));
    assert(file->f_extent->next->header.fst_rec_ptr == SECTION_SIZE - entity_rec_size(s_entity));
    assert(file->f_extent->next->header.lst_itm_ptr == sizeof(sect_head_entity_t) + entity_itm_size(s_entity));
    assert(file->f_extent->next->header.next_ptr == 0);
    assert(file->f_extent->next->header.sect_off == sizeof(file_head_t) + SECTION_SIZE);

    entity_t *obj_o_entity = entity_new();
    json_t *obj_o_json = json_new();
    sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, obj_wrt_addr), obj_o_entity, obj_o_json);

    entity_t *f_o_entity = entity_new();
    json_t *f_o_json = json_new();
    sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, obj_o_entity->fam_addr.bro_ptr), f_o_entity, f_o_json);

    entity_t *s_o_entity = entity_new();
    json_t *s_o_json = json_new();
    sect_ext_read(file->f_extent, sect_head_get_sectoff(&file->f_extent->header, obj_o_entity->fam_addr.son_ptr), s_o_entity, s_o_json);

    assert(f_o_entity->key_ptr == SECTION_SIZE - f_entity->key_size);
    assert(f_o_entity->key_size == f_entity->key_size);
    assert(f_o_entity->val_ptr == SECTION_SIZE - f_entity->key_size - f_entity->val_size);
    assert(f_o_entity->val_size == f_entity->val_size);
    assert(f_o_entity->rec_size == f_entity->key_size + f_entity->val_size);
    assert(f_o_entity->type == f_entity->type);
    assert(f_o_entity->fam_addr.dad_ptr == 0);
    assert(f_o_entity->fam_addr.bro_ptr == 0);
    assert(f_o_entity->fam_addr.son_ptr == 0);

    assert(obj_o_entity->key_ptr == SECTION_SIZE - f_entity->key_size - f_entity->val_size - obj_entity->key_size);
    assert(obj_o_entity->key_size == obj_entity->key_size);
    assert(obj_o_entity->val_ptr == 0);
    assert(obj_o_entity->val_size == 0);
    assert(obj_o_entity->rec_size == obj_entity->key_size);
    assert(obj_o_entity->type == obj_entity->type);
    assert(obj_o_entity->fam_addr.dad_ptr == 0);
    assert(obj_o_entity->fam_addr.bro_ptr == sizeof(file_head_t) + sizeof(sect_head_entity_t) + 8000);
    assert(obj_o_entity->fam_addr.son_ptr == sizeof(file_head_t) + SECTION_SIZE + sizeof(sect_head_entity_t));

    assert(s_o_entity->key_ptr == SECTION_SIZE - s_entity->key_size);
    assert(s_o_entity->key_size == s_entity->key_size);
    assert(s_o_entity->val_ptr == SECTION_SIZE - s_entity->key_size - s_entity->val_size);
    assert(s_o_entity->val_size == s_entity->val_size);
    assert(s_o_entity->rec_size == s_entity->key_size + s_entity->val_size);
    assert(s_o_entity->type == s_entity->type);
    assert(s_o_entity->fam_addr.dad_ptr == obj_wrt_addr);
    assert(s_o_entity->fam_addr.bro_ptr == 0);
    assert(s_o_entity->fam_addr.son_ptr == 0);

    json_dtor(f_json);
    json_dtor(obj_json);
    json_dtor(f_o_json);
    json_dtor(s_o_json);
    json_dtor(obj_o_json);

    entity_dtor(f_entity);
    entity_dtor(s_entity);
    entity_dtor(obj_entity);
    entity_dtor(f_o_entity);
    entity_dtor(s_o_entity);
    entity_dtor(obj_o_entity);

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
}