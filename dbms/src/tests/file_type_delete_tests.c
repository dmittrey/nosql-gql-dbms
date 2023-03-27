#include "tests/test.h"

#include "file/file.h"
#include "file/file_p.h"

static const char *test_file_name = "test.bin";

/*
1) Удаление без атрибутов из сущ секции
2) Удаление с атрибутами из сущ секции
3) Удаление без атрибутов из несущ секции
*/

static Status File_DeleteTypeWithoutAtr_Successful()
{
    FILE *filp = fopen(test_file_name, "w+");

    File *file = file_new();
    file_ctor(file, filp);

    Fileoff wrt_adr;
    TYPE_INIT(V_type, "V");
    file_add_type(file, V_type, &wrt_adr);

    file_delete_type(file, V_type->name);

    assert(file->header.lst_sect_ptr == sizeof(File_head_entity) + SECTION_SIZE);

    File_head_entity *file_head_entity = my_malloc(File_head_entity);
    file_head_read((File_head *)file, 0, sizeof(File_head_entity), file_head_entity);
    assert(file_head_entity->lst_sect_ptr == sizeof(File_head_entity) + SECTION_SIZE);
    free(file_head_entity);

    assert(file->f_types->head->header.free_space == SECTION_SIZE - sizeof(Sect_head_entity));
    assert(file->f_types->head->header.next_ptr == 0); // Next section is undefined
    assert(file->f_types->head->header.lst_itm_ptr == sizeof(Sect_head_entity));
    assert(file->f_types->head->header.fst_rec_ptr == SECTION_SIZE);
    assert(file->f_types->head->header.file_offset == sizeof(File_head_entity));

    Sect_head_entity *head_entity = my_malloc(Sect_head_entity);
    sect_head_read((Sect_head *)file->f_types->head, 0, sizeof(Sect_head_entity), head_entity);
    assert(head_entity->free_space == SECTION_SIZE - sizeof(Sect_head_entity));
    assert(head_entity->next_ptr == 0); // Next section is undefined
    assert(head_entity->lst_itm_ptr == sizeof(Sect_head_entity));
    assert(head_entity->fst_rec_ptr == SECTION_SIZE);
    free(head_entity);

    type_dtor(V_type);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static Status File_DeleteTypeWithAtr_Successful()
{
    FILE *filp = fopen(test_file_name, "w+");

    File *file = file_new();
    file_ctor(file, filp);

    Fileoff wrt_adr;
    TYPE_INIT(V_type, "V");

    ATR_INIT(int_attr, "int", TYPE_INT32);
    type_add_atr(V_type, int_attr);
    ATR_INIT(float_attr, "float", TYPE_FLOAT);
    type_add_atr(V_type, float_attr);
    ATR_INIT(bool_attr, "bool", TYPE_BOOL);
    type_add_atr(V_type, bool_attr);
    ATR_INIT(string_attr, "string", TYPE_STRING);
    type_add_atr(V_type, string_attr);

    file_add_type(file, V_type, &wrt_adr);

    file_delete_type(file, V_type->name);

    assert(file->header.lst_sect_ptr == sizeof(File_head_entity) + SECTION_SIZE);

    File_head_entity *file_head_entity = my_malloc(File_head_entity);
    file_head_read((File_head *)file, 0, sizeof(File_head_entity), file_head_entity);
    assert(file_head_entity->lst_sect_ptr == sizeof(File_head_entity) + SECTION_SIZE);
    free(file_head_entity);

    assert(file->f_types->head->header.free_space == SECTION_SIZE - sizeof(Sect_head_entity));
    assert(file->f_types->head->header.next_ptr == 0); // Next section is undefined
    assert(file->f_types->head->header.lst_itm_ptr == sizeof(Sect_head_entity));
    assert(file->f_types->head->header.fst_rec_ptr == SECTION_SIZE);
    assert(file->f_types->head->header.file_offset == sizeof(File_head_entity));

    Sect_head_entity *head_entity = my_malloc(Sect_head_entity);
    sect_head_read((Sect_head *)file->f_types->head, 0, sizeof(Sect_head_entity), head_entity);
    assert(head_entity->free_space == SECTION_SIZE - sizeof(Sect_head_entity));
    assert(head_entity->next_ptr == 0); // Next section is undefined
    assert(head_entity->lst_itm_ptr == sizeof(Sect_head_entity));
    assert(head_entity->fst_rec_ptr == SECTION_SIZE);
    free(head_entity);

    type_dtor(V_type);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

Status File_DeleteTypeWithoutAtrFromNextSect_Successful()
{
    FILE *filp = fopen(test_file_name, "w+");

    File *file = file_new();
    file_ctor(file, filp);

    Sect_types *fil_types = sect_types_new();
    file_add_sect_types(file, fil_types);
    fil_types->header.free_space = 0;

    Fileoff wrt_adr;
    TYPE_INIT(V_type, "V");

    ATR_INIT(int_attr, "int", TYPE_INT32);
    type_add_atr(V_type, int_attr);
    ATR_INIT(float_attr, "float", TYPE_FLOAT);
    type_add_atr(V_type, float_attr);
    ATR_INIT(bool_attr, "bool", TYPE_BOOL);
    type_add_atr(V_type, bool_attr);
    ATR_INIT(string_attr, "string", TYPE_STRING);
    type_add_atr(V_type, string_attr);

    file_add_type(file, V_type, &wrt_adr);

    file_delete_type(file, V_type->name);

    assert(file->header.lst_sect_ptr == sizeof(File_head_entity) + 2 * SECTION_SIZE);

    File_head_entity *f_head_entity = my_malloc(File_head_entity);
    file_head_read((File_head *)file, 0, sizeof(File_head_entity), f_head_entity);
    assert(f_head_entity->lst_sect_ptr == sizeof(File_head_entity) + 2 * SECTION_SIZE);
    free(f_head_entity);

    assert(file->f_types->head->next->header.free_space == SECTION_SIZE - sizeof(Sect_head_entity));
    assert(file->f_types->head->next->header.next_ptr == 0); // Next section is undefined
    assert(file->f_types->head->next->header.lst_itm_ptr == sizeof(Sect_head_entity));
    assert(file->f_types->head->next->header.fst_rec_ptr == SECTION_SIZE);
    assert(file->f_types->head->next->header.file_offset == sizeof(File_head_entity) + SECTION_SIZE);

    Sect_head_entity *head_entity = my_malloc(Sect_head_entity);
    sect_head_read((Sect_head *)file->f_types->head->next, 0, sizeof(Sect_head_entity), head_entity);
    assert(head_entity->free_space == SECTION_SIZE - sizeof(Sect_head_entity));
    assert(head_entity->next_ptr == 0); // Next section is undefined
    assert(head_entity->lst_itm_ptr == sizeof(Sect_head_entity));
    assert(head_entity->fst_rec_ptr == SECTION_SIZE);
    free(head_entity);

    type_dtor(V_type);

    file_dtor(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_file_delete_type()
{
    assert(File_DeleteTypeWithoutAtr_Successful() == OK);
    assert(File_DeleteTypeWithAtr_Successful() == OK);
    assert(File_DeleteTypeWithoutAtrFromNextSect_Successful() == OK);
}