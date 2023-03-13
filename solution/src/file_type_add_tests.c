#include <assert.h>

#include "string.h"

#include "memory/file/file.h"

#include "memory/type/type.h"

#include "memory/section/types.h"

static const char *test_file_name = "test.bin";

/*
1) Добавление типа без атрибутов в пустой файл
2) Добавление типа с атрибутами в пустой файл
3) Добавление типа без атрибутов в файл с одной заполненной секцией
4) Добавление типа с атрибутами в файл с одной заполненной секцией
*/

status_t File_AddTypeIntoEmptyFile_Successful()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    TYPE_INIT(V_type, "V");
    file_add_type(file, V_type);

    type_dtor(V_type);

    // Assert add types section
    assert(file->header.lst_sect_ptr == sizeof(file_head_t) + SECTION_SIZE);
    assert(file->f_types->next == 0);

    // Assert add type into section
    assert(file->f_types->header.fst_rec_ptr == SECTION_SIZE - 1);
    assert(file->f_types->header.lst_itm_ptr == sizeof(sect_head_entity_t) + sizeof(type_entity_t));
    assert(file->f_types->header.free_space == file->f_types->header.fst_rec_ptr - file->f_types->header.lst_itm_ptr);
    assert(file->f_types->header.next_ptr == 0);
    assert(file->f_types->header.sect_off == sizeof(file_head_t));

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t File_AddTypeWithAttrIntoEmptyFile_Successful()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    TYPE_INIT(V_type, "V");

    ATR_INIT(int_attr, "int", TYPE_INT32);
    type_add_atr(V_type, int_attr);
    ATR_INIT(float_attr, "float", TYPE_FLOAT);
    type_add_atr(V_type, float_attr);
    ATR_INIT(bool_attr, "bool", TYPE_BOOL);
    type_add_atr(V_type, bool_attr);
    ATR_INIT(string_attr, "string", TYPE_STRING);
    type_add_atr(V_type, string_attr);

    file_add_type(file, V_type);

    // Assert add types section
    assert(file->header.lst_sect_ptr == sizeof(file_head_t) + SECTION_SIZE);
    assert(file->f_types->next == 0);

    // Assert add type into section
    assert(file->f_types->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t) - sizeof(type_entity_t) - 1 * sizeof(char) - 4 * sizeof(attr_entity_t) - 18 * sizeof(char));
    assert(file->f_types->header.lst_itm_ptr == sizeof(sect_head_entity_t) + sizeof(type_entity_t) + 4 * sizeof(attr_entity_t));
    assert(file->f_types->header.fst_rec_ptr == SECTION_SIZE - 1 * sizeof(char) - 18 * sizeof(char));
    assert(file->f_types->header.next_ptr == 0);
    assert(file->f_types->header.sect_off == sizeof(file_head_t));

    type_entity_t *rd_type_ent = type_entity_new();
    sect_type_rd_ent(file->f_types, sizeof(sect_head_entity_t), rd_type_ent);
    assert(rd_type_ent->attr_cnt == V_type->attr_list->count);
    assert(rd_type_ent->name_ptr == SECTION_SIZE - 1 * sizeof(char));
    assert(rd_type_ent->name_size == V_type->name->cnt);

    attr_entity_t *rd_attr_ent = attr_entity_new();
    sect_type_rd_atr(file->f_types, sizeof(sect_head_entity_t) + sizeof(type_entity_t), rd_attr_ent);
    assert(rd_attr_ent->name_size == int_attr->name->cnt);
    assert(rd_attr_ent->name_ptr == SECTION_SIZE - 1 * sizeof(char) - int_attr->name->cnt * sizeof(char));
    assert(rd_attr_ent->attr_type == TYPE_INT32);

    sect_type_rd_atr(file->f_types, sizeof(sect_head_entity_t) + sizeof(type_entity_t) + sizeof(attr_entity_t), rd_attr_ent);
    assert(rd_attr_ent->name_size == float_attr->name->cnt);
    assert(rd_attr_ent->name_ptr == SECTION_SIZE - 1 * sizeof(char) - (int_attr->name->cnt + float_attr->name->cnt) * sizeof(char));
    assert(rd_attr_ent->attr_type == TYPE_FLOAT);

    sect_type_rd_atr(file->f_types, sizeof(sect_head_entity_t) + sizeof(type_entity_t) + 2 * sizeof(attr_entity_t), rd_attr_ent);
    assert(rd_attr_ent->name_size == bool_attr->name->cnt);
    assert(rd_attr_ent->name_ptr == SECTION_SIZE - 1 * sizeof(char) - (int_attr->name->cnt + float_attr->name->cnt + bool_attr->name->cnt) * sizeof(char));
    assert(rd_attr_ent->attr_type == TYPE_BOOL);

    sect_type_rd_atr(file->f_types, sizeof(sect_head_entity_t) + sizeof(type_entity_t) + 3 * sizeof(attr_entity_t), rd_attr_ent);
    assert(rd_attr_ent->name_size == string_attr->name->cnt);
    assert(rd_attr_ent->name_ptr == SECTION_SIZE - 1 * sizeof(char) - (int_attr->name->cnt + float_attr->name->cnt + bool_attr->name->cnt + string_attr->name->cnt) * sizeof(char));
    assert(rd_attr_ent->attr_type == TYPE_STRING);

    type_dtor(V_type);

    type_entity_dtor(rd_type_ent);
    attr_entity_dtor(rd_attr_ent);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t File_AddTypeIntoFileWithFilledSection_Successful()
{
    FILE *filp = fopen(test_file_name, "w+");

    file_t *file = file_new();
    file_ctor(file, filp);

    sect_type_t *fil_types = sect_type_new();
    file_add_sect_types(file, fil_types);
    sect_head_shift_lst_itm_ptr(&fil_types->header, 8160);

    TYPE_INIT(V_type, "V");
    file_add_type(file, V_type);

    type_dtor(V_type);

    // Assert add types section
    assert(file->header.lst_sect_ptr == sizeof(file_head_t) + 2 * SECTION_SIZE);
    assert(file->f_types->header.next_ptr == sizeof(file_head_t) + SECTION_SIZE);

    // Assert add type into section
    assert(file->f_types->next->header.fst_rec_ptr == SECTION_SIZE - 1);
    assert(file->f_types->next->header.lst_itm_ptr == sizeof(sect_head_entity_t) + sizeof(type_entity_t));
    assert(file->f_types->next->header.free_space == file->f_types->next->header.fst_rec_ptr - file->f_types->next->header.lst_itm_ptr);
    assert(file->f_types->next->header.next_ptr == 0);
    assert(file->f_types->next->header.sect_off == sizeof(file_head_t) + SECTION_SIZE);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_file_add_type()
{
    assert(File_AddTypeIntoEmptyFile_Successful() == OK);
    assert(File_AddTypeWithAttrIntoEmptyFile_Successful() == OK);
    assert(File_AddTypeIntoFileWithFilledSection_Successful() == OK);
}