#include <assert.h>

#include "file/file.h"
#include "file/file_p.h"

static const char *test_file_name = "test.bin";

/*
1) Добавление типа без атрибутов в пустой файл
2) Добавление типа с атрибутами в пустой файл
3) Добавление типа без атрибутов в файл с одной заполненной секцией
4) Добавление типа с атрибутами в файл с одной заполненной секцией
*/

static Status File_AddTypeIntoEmptyFile_Successful()
{
    FILE *filp = fopen(test_file_name, "w+");

    File *file = file_new();
    file_ctor(file, filp);

    Fileoff wrt_foff;
    TYPE_INIT(V_type, "V");
    file_add_type(file, V_type, &wrt_foff);

    assert(file->header.lst_sect_ptr == sizeof(File_head_entity) + SECTION_SIZE);

    File_head_entity *f_head_entity = my_malloc(File_head_entity);
    file_head_read((File_head *)file, 0, sizeof(File_head_entity), f_head_entity);
    assert(f_head_entity->lst_sect_ptr == sizeof(File_head_entity) + SECTION_SIZE);
    free(f_head_entity);

    assert(file->f_types->head->header.free_space == SECTION_SIZE - sizeof(Sect_head_entity) - type_ph_sz(V_type));
    assert(file->f_types->head->header.next_ptr == 0); // Next section is undefined
    assert(file->f_types->head->header.lst_itm_ptr == sizeof(Sect_head_entity) + sizeof(Type_entity));
    assert(file->f_types->head->header.fst_rec_ptr == SECTION_SIZE - V_type->name->cnt);
    assert(file->f_types->head->header.file_offset == sizeof(File_head_entity));

    Sect_head_entity *head_entity = my_malloc(Sect_head_entity);
    sect_head_read((Sect_head *)file->f_types->head, 0, sizeof(Sect_head_entity), head_entity);
    assert(head_entity->free_space == SECTION_SIZE - sizeof(Sect_head_entity) - type_ph_sz(V_type));
    assert(head_entity->next_ptr == 0); // Next section is undefined
    assert(head_entity->lst_itm_ptr == sizeof(Sect_head_entity) + sizeof(Type_entity));
    assert(head_entity->fst_rec_ptr == SECTION_SIZE - V_type->name->cnt);
    free(head_entity);

    Sectoff soff;
    Sect_types *s_ext;
    Type *t = type_new();
    file_find_type(file, V_type->name, t, &soff, &s_ext);
    assert(t->attr_list->count == 0);
    assert(string_cmp(t->name, V_type->name) == 0);
    assert(t->next == NULL);

    type_dtor(t);
    type_dtor(V_type);
    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static Status File_AddTypeWithAttrIntoEmptyFile_Successful()
{
    FILE *filp = fopen(test_file_name, "w+");

    File *file = file_new();
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

    Fileoff wrt_adr;
    file_add_type(file, V_type, &wrt_adr);

    assert(file->header.lst_sect_ptr == sizeof(File_head_entity) + SECTION_SIZE);

    File_head_entity *f_head_entity = my_malloc(File_head_entity);
    file_head_read((File_head *)file, 0, sizeof(File_head_entity), f_head_entity);
    assert(f_head_entity->lst_sect_ptr == sizeof(File_head_entity) + SECTION_SIZE);
    free(f_head_entity);

    assert(file->f_types->head->header.free_space == SECTION_SIZE - sizeof(Sect_head_entity) - type_ph_sz(V_type));
    assert(file->f_types->head->header.next_ptr == 0); // Next section is undefined
    assert(file->f_types->head->header.lst_itm_ptr == sizeof(Sect_head_entity) + type_itm_sz(V_type));
    assert(file->f_types->head->header.fst_rec_ptr == SECTION_SIZE - type_rec_sz(V_type));
    assert(file->f_types->head->header.file_offset == sizeof(File_head_entity));

    Sect_head_entity *head_entity = my_malloc(Sect_head_entity);
    sect_head_read((Sect_head *)file->f_types->head, 0, sizeof(Sect_head_entity), head_entity);
    assert(head_entity->free_space == SECTION_SIZE - sizeof(Sect_head_entity) - type_ph_sz(V_type));
    assert(head_entity->next_ptr == 0); // Next section is undefined
    assert(head_entity->lst_itm_ptr == sizeof(Sect_head_entity) + type_itm_sz(V_type));
    assert(head_entity->fst_rec_ptr == SECTION_SIZE - type_rec_sz(V_type));
    free(head_entity);

    Sectoff soff;
    Sect_types *s_ext;
    Type *t = type_new();
    file_find_type(file, V_type->name, t, &soff, &s_ext);
    assert(t->attr_list->count == 4);
    assert(string_cmp(t->name, V_type->name) == 0);
    assert(attr_cmp(t->attr_list->head, V_type->attr_list->head) == 0);
    list_Attr_del_fst(t->attr_list);
    list_Attr_del_fst(V_type->attr_list);
    assert(attr_cmp(t->attr_list->head, V_type->attr_list->head) == 0);
    list_Attr_del_fst(t->attr_list);
    list_Attr_del_fst(V_type->attr_list);
    assert(attr_cmp(t->attr_list->head, V_type->attr_list->head) == 0);
    list_Attr_del_fst(t->attr_list);
    list_Attr_del_fst(V_type->attr_list);
    assert(attr_cmp(t->attr_list->head, V_type->attr_list->head) == 0);
    list_Attr_del_fst(t->attr_list);
    list_Attr_del_fst(V_type->attr_list);

    type_dtor(V_type);
    type_dtor(t);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static Status File_AddTypeIntoFileWithFilledSection_Successful()
{
    FILE *filp = fopen(test_file_name, "w+");

    File *file = file_new();
    file_ctor(file, filp);

    Sect_types *fil_types = sect_types_new();
    file_add_sect_types(file, fil_types);
    sect_head_shift_lip((Sect_head *)fil_types, 8159);

    Fileoff wrt_adr;
    TYPE_INIT(V_type, "V");
    file_add_type(file, V_type, &wrt_adr);

    assert(file->header.lst_sect_ptr == sizeof(File_head_entity) + 2 * SECTION_SIZE);

    File_head_entity *f_head_entity = my_malloc(File_head_entity);
    file_head_read((File_head *)file, 0, sizeof(File_head_entity), f_head_entity);
    assert(f_head_entity->lst_sect_ptr == sizeof(File_head_entity) + 2 * SECTION_SIZE);
    free(f_head_entity);

    assert(file->f_types->head->next->header.free_space == SECTION_SIZE - sizeof(Sect_head_entity) - type_ph_sz(V_type));
    assert(file->f_types->head->next->header.next_ptr == 0); // Next section is undefined
    assert(file->f_types->head->next->header.lst_itm_ptr == sizeof(Sect_head_entity) + sizeof(Type_entity));
    assert(file->f_types->head->next->header.fst_rec_ptr == SECTION_SIZE - V_type->name->cnt);
    assert(file->f_types->head->next->header.file_offset == sizeof(File_head_entity) + SECTION_SIZE);

    Sect_head_entity *head_entity = my_malloc(Sect_head_entity);
    sect_head_read((Sect_head *)file->f_types->head->next, 0, sizeof(Sect_head_entity), head_entity);
    assert(head_entity->free_space == SECTION_SIZE - sizeof(Sect_head_entity) - type_ph_sz(V_type));
    assert(head_entity->next_ptr == 0); // Next section is undefined
    assert(head_entity->lst_itm_ptr == sizeof(Sect_head_entity) + type_itm_sz(V_type));
    assert(head_entity->fst_rec_ptr == SECTION_SIZE - type_rec_sz(V_type));
    free(head_entity);

    Sectoff soff;
    Sect_types *s_ext;
    Type *t = type_new();
    file_find_type(file, V_type->name, t, &soff, &s_ext);
    assert(t->attr_list->count == 0);
    assert(string_cmp(t->name, V_type->name) == 0);

    type_dtor(V_type);
    type_dtor(t);

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