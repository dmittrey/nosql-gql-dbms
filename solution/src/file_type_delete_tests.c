// #include <assert.h>

// #include "memory/file/file.h"

// #include "memory/section/types_p.h"

// static const char *test_file_name = "test.bin";

// /*
// 1) Удаление без атрибутов из сущ секции
// 2) Удаление с атрибутами из сущ секции
// 3) Удаление без атрибутов из несущ секции
// */

// status_t File_DeleteTypeWithoutAtr_Successful()
// {
//     FILE *filp = fopen(test_file_name, "w+");

//     file_t *file = file_new();
//     file_ctor(file, filp);

//     TYPE_INIT(V_type, "V");
//     file_add_type(file, V_type);

//     STR_INIT(q_name, "V");
//     file_delete_type(file, q_name);

//     // Assert add types section
//     assert(file->header.lst_sect_ptr == sizeof(file_head_t) + SECTION_SIZE);
//     assert(file->f_types->next == 0);

//     // Assert add type into section
//     assert(file->f_types->header.fst_rec_ptr == SECTION_SIZE);
//     assert(file->f_types->header.lst_itm_ptr == sizeof(sect_head_entity_t));
//     assert(file->f_types->header.free_space == file->f_types->header.fst_rec_ptr - file->f_types->header.lst_itm_ptr);
//     assert(file->f_types->header.next_ptr == 0);
//     assert(file->f_types->header.sect_off == sizeof(file_head_t));

//     type_dtor(V_type);

//     string_dtor(q_name);

//     file_dtor(file);
//     DO_OR_FAIL(remove(test_file_name));

//     return OK;
// }

// status_t File_DeleteTypeWithAtr_Successful()
// {
//     FILE *filp = fopen(test_file_name, "w+");

//     file_t *file = file_new();
//     file_ctor(file, filp);

//     TYPE_INIT(V_type, "V");

//     ATR_INIT(int_attr, "int", TYPE_INT32);
//     type_add_atr(V_type, int_attr);
//     ATR_INIT(float_attr, "float", TYPE_FLOAT);
//     type_add_atr(V_type, float_attr);
//     ATR_INIT(bool_attr, "bool", TYPE_BOOL);
//     type_add_atr(V_type, bool_attr);
//     ATR_INIT(string_attr, "string", TYPE_STRING);
//     type_add_atr(V_type, string_attr);

//     file_add_type(file, V_type);

//     STR_INIT(q_name, "V");
//     file_delete_type(file, q_name);

//     // Assert add types section
//     assert(file->header.lst_sect_ptr == sizeof(file_head_t) + SECTION_SIZE);
//     assert(file->f_types->next == 0);

//     // Assert add type into section
//     assert(file->f_types->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t));
//     assert(file->f_types->header.lst_itm_ptr == sizeof(sect_head_entity_t));
//     assert(file->f_types->header.fst_rec_ptr == SECTION_SIZE);
//     assert(file->f_types->header.next_ptr == 0);
//     assert(file->f_types->header.sect_off == sizeof(file_head_t));

//     type_dtor(V_type);

//     string_dtor(q_name);

//     file_dtor(file);
//     DO_OR_FAIL(remove(test_file_name));

//     return OK;
// }

// status_t File_DeleteTypeWithoutAtrFromNextSect_Successful()
// {
//     FILE *filp = fopen(test_file_name, "w+");

//     file_t *file = file_new();
//     file_ctor(file, filp);

//     sect_type_t *fil_types = sect_type_new();
//     file_add_sect_types(file, fil_types);
//     sect_head_shift_lip(&fil_types->header, 8160);

//     TYPE_INIT(V_type, "V");
//     file_add_type(file, V_type);

//     STR_INIT(q_name, "V");
//     file_delete_type(file, q_name);

//     // Assert add types section
//     assert(file->header.lst_sect_ptr == sizeof(file_head_t) + 2 * SECTION_SIZE);
//     assert(file->f_types->header.next_ptr == sizeof(file_head_t) + SECTION_SIZE);

//     // Assert add type into section
//     assert(file->f_types->next->header.fst_rec_ptr == SECTION_SIZE);
//     assert(file->f_types->next->header.lst_itm_ptr == sizeof(sect_head_entity_t));
//     assert(file->f_types->next->header.free_space == file->f_types->next->header.fst_rec_ptr - file->f_types->next->header.lst_itm_ptr);
//     assert(file->f_types->next->header.next_ptr == 0);
//     assert(file->f_types->next->header.sect_off == sizeof(file_head_t) + SECTION_SIZE);

//     type_dtor(V_type);

//     string_dtor(q_name);

//     file_dtor(file);
//     DO_OR_FAIL(remove(test_file_name));

//     return OK;
// }

// void test_file_delete_type()
// {
//     assert(File_DeleteTypeWithoutAtr_Successful() == OK);
//     assert(File_DeleteTypeWithAtr_Successful() == OK);
//     assert(File_DeleteTypeWithoutAtrFromNextSect_Successful() == OK);
// }