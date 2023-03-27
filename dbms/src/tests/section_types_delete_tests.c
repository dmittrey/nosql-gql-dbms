#include "tests/test.h"

#include "section/types.h"
#include "section/types_p.h"

static const char *test_file_name = "test.bin";

/*
1) Удаление типа без атрибутов на границе
2) Удаление типа без атрибутов не на границе
3) Удаление внутри а потом на границе(ptrs promotion check)
4) Удаление типа с атрибутом на границе
5) Удаление типа с атрибутом внутри
*/

Status SectionTypes_DeleteTypeWithoutAttributesOnBorder_DeleteAndShiftPtrs()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_types *types = sect_types_new();
    sect_types_ctor(types, 0, file);

    TYPE_INIT(wr_type, "V");

    Sectoff wrt_adr;
    sect_types_write(types, wr_type, &wrt_adr);

    sect_types_delete(types, wrt_adr);
    assert(types->header.free_space == (SECTION_SIZE - sizeof(Sect_head_entity)));
    assert(types->header.next_ptr == 0); // Next section is undefined
    assert(types->header.lst_itm_ptr == sizeof(Sect_head_entity));
    assert(types->header.fst_rec_ptr == SECTION_SIZE);
    assert(types->header.file_offset == 0);

    Sect_head_entity *head_entity = my_malloc(Sect_head_entity);
    sect_head_read((Sect_head *)types, 0, sizeof(Sect_head_entity), head_entity);
    assert(head_entity->free_space == (SECTION_SIZE - sizeof(Sect_head_entity)));
    assert(head_entity->next_ptr == 0); // Next section is undefined
    assert(head_entity->lst_itm_ptr == sizeof(Sect_head_entity));
    assert(head_entity->fst_rec_ptr == SECTION_SIZE);
    free(head_entity);

    Type_entity *zero_ent = type_entity_new();
    Type_entity *temp_ent = type_entity_new();
    sect_types_rd_type(types, wrt_adr, temp_ent);
    assert(memcmp(zero_ent, temp_ent, sizeof(Type_entity)) == 0);

    void *rec_zr = calloc(wr_type->name->cnt, sizeof(char));
    void *rec = my_malloc_array(char, wr_type->name->cnt);
    sect_head_read((Sect_head *)types, SECTION_SIZE - wr_type->name->cnt, wr_type->name->cnt, rec);
    assert(memcmp(rec_zr, rec, wr_type->name->cnt) == 0);

    sect_types_dtor(types);
    type_dtor(wr_type);
    type_entity_dtor(zero_ent);
    type_entity_dtor(temp_ent);
    free(rec_zr);
    free(rec);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

Status SectionTypes_DeleteInnerTypeWithoutAttributes_Delete()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_types *types = sect_types_new();
    sect_types_ctor(types, 0, file);

    Sectoff wrt_adr;
    TYPE_INIT(V_type, "V");
    sect_types_write(types, V_type, &wrt_adr);

    TYPE_INIT(K_type, "K");
    sect_types_write(types, K_type, &wrt_adr);

    sect_types_delete(types, sizeof(Sect_head_entity));

    assert(types->header.free_space == (SECTION_SIZE - sizeof(Sect_head_entity) - type_ph_sz(V_type) - type_ph_sz(K_type)));
    assert(types->header.next_ptr == 0); // Next section is undefined
    assert(types->header.lst_itm_ptr == sizeof(Sect_head_entity) + type_itm_sz(V_type) + type_itm_sz(K_type));
    assert(types->header.fst_rec_ptr == SECTION_SIZE - type_rec_sz(V_type) - type_rec_sz(K_type));
    assert(types->header.file_offset == 0);

    Sect_head_entity *head_entity = my_malloc(Sect_head_entity);
    sect_head_read((Sect_head *)types, 0, sizeof(Sect_head_entity), head_entity);
    assert(head_entity->free_space == (SECTION_SIZE - sizeof(Sect_head_entity) - type_ph_sz(V_type) - type_ph_sz(K_type)));
    assert(head_entity->next_ptr == 0); // Next section is undefined
    assert(head_entity->lst_itm_ptr == sizeof(Sect_head_entity) + type_itm_sz(V_type) + type_itm_sz(K_type));
    assert(head_entity->fst_rec_ptr == SECTION_SIZE - type_rec_sz(V_type) - type_rec_sz(K_type));
    free(head_entity);

    Type_entity *zero_ent = type_entity_new();
    Type_entity *temp_ent = type_entity_new();
    sect_types_rd_type(types, sizeof(Sect_head_entity), temp_ent);
    assert(memcmp(zero_ent, temp_ent, sizeof(Type_entity)) == 0);

    void *rec_zr = calloc(V_type->name->cnt, sizeof(char));
    void *rec = my_malloc_array(char, V_type->name->cnt);
    sect_head_read((Sect_head *)types, SECTION_SIZE - V_type->name->cnt, V_type->name->cnt, rec);
    assert(memcmp(rec_zr, rec, V_type->name->cnt) == 0);

    sect_types_dtor(types);
    type_dtor(V_type);
    type_dtor(K_type);
    type_entity_dtor(zero_ent);
    type_entity_dtor(temp_ent);
    free(rec_zr);
    free(rec);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

Status SectionTypes_DeleteInnerTypeWithoutAttributesAndAfterBorder_DeleteAllAndShiftPtrs()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_types *types = sect_types_new();
    sect_types_ctor(types, 0, file);

    Sectoff wrt_adr;
    TYPE_INIT(V_type, "V");
    sect_types_write(types, V_type, &wrt_adr);

    TYPE_INIT(K_type, "K");
    sect_types_write(types, K_type, &wrt_adr);

    sect_types_delete(types, sizeof(Sect_head_entity));
    sect_types_delete(types, wrt_adr);

    assert(types->header.free_space == (SECTION_SIZE - sizeof(Sect_head_entity)));
    assert(types->header.next_ptr == 0); // Next section is undefined
    assert(types->header.lst_itm_ptr == sizeof(Sect_head_entity));
    assert(types->header.fst_rec_ptr == SECTION_SIZE);
    assert(types->header.file_offset == 0);

    Sect_head_entity *head_entity = my_malloc(Sect_head_entity);
    sect_head_read((Sect_head *)types, 0, sizeof(Sect_head_entity), head_entity);
    assert(head_entity->free_space == (SECTION_SIZE - sizeof(Sect_head_entity)));
    assert(head_entity->next_ptr == 0); // Next section is undefined
    assert(head_entity->lst_itm_ptr == sizeof(Sect_head_entity));
    assert(head_entity->fst_rec_ptr == SECTION_SIZE);
    free(head_entity);

    Type_entity *zero_ent = type_entity_new();
    Type_entity *temp_ent = type_entity_new();
    sect_types_rd_type(types, sizeof(Sect_head_entity), temp_ent);
    assert(memcmp(zero_ent, temp_ent, sizeof(Type_entity)) == 0);
    sect_types_rd_type(types, wrt_adr, temp_ent);
    assert(memcmp(zero_ent, temp_ent, sizeof(Type_entity)) == 0);

    void *rec_zr = calloc(V_type->name->cnt, sizeof(char));
    void *rec = my_malloc_array(char, V_type->name->cnt);
    sect_head_read((Sect_head *)types, SECTION_SIZE - V_type->name->cnt, V_type->name->cnt, rec);
    assert(memcmp(rec_zr, rec, V_type->name->cnt) == 0);
    sect_head_read((Sect_head *)types, SECTION_SIZE - V_type->name->cnt - K_type->name->cnt, K_type->name->cnt, rec);
    assert(memcmp(rec_zr, rec, K_type->name->cnt) == 0);

    sect_types_dtor(types);
    type_dtor(V_type);
    type_dtor(K_type);
    type_entity_dtor(zero_ent);
    type_entity_dtor(temp_ent);
    free(rec_zr);
    free(rec);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

Status SectionTypes_DeleteTypeWithAttributesOnBorder_DeleteAndShiftPtrs()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_types *types = sect_types_new();
    sect_types_ctor(types, 0, file);

    TYPE_INIT(t, "V");

    ATR_INIT(bool_atr, "bool", TYPE_BOOL);
    type_add_atr(t, bool_atr);
    ATR_INIT(int32_atr, "int32", TYPE_INT32);
    type_add_atr(t, int32_atr);
    ATR_INIT(str_atr, "str", TYPE_STRING);
    type_add_atr(t, str_atr);
    ATR_INIT(float_atr, "float", TYPE_FLOAT);
    type_add_atr(t, float_atr);

    Sectoff wrt_adr;
    sect_types_write(types, t, &wrt_adr);

    sect_types_delete(types, wrt_adr);

    assert(types->header.free_space == (SECTION_SIZE - sizeof(Sect_head_entity)));
    assert(types->header.next_ptr == 0); // Next section is undefined
    assert(types->header.lst_itm_ptr == sizeof(Sect_head_entity));
    assert(types->header.fst_rec_ptr == SECTION_SIZE);
    assert(types->header.file_offset == 0);

    Sect_head_entity *head_entity = my_malloc(Sect_head_entity);
    sect_head_read((Sect_head *)types, 0, sizeof(Sect_head_entity), head_entity);
    assert(head_entity->free_space == (SECTION_SIZE - sizeof(Sect_head_entity)));
    assert(head_entity->next_ptr == 0); // Next section is undefined
    assert(head_entity->lst_itm_ptr == sizeof(Sect_head_entity));
    assert(head_entity->fst_rec_ptr == SECTION_SIZE);
    free(head_entity);

    char *zr_itm = calloc(type_itm_sz(t), sizeof(char));
    char *rd_itm = my_malloc_array(char, type_itm_sz(t));
    sect_head_read((Sect_head *)types, wrt_adr, type_itm_sz(t), rd_itm);
    assert(memcmp(rd_itm, zr_itm, type_itm_sz(t)) == 0);

    char *zr_rec = calloc(type_rec_sz(t), sizeof(char));
    char *rd_rec = my_malloc_array(char, type_rec_sz(t));
    sect_head_read((Sect_head *)types, SECTION_SIZE - type_rec_sz(t), type_rec_sz(t), rd_itm);
    assert(memcmp(rd_itm, zr_itm, type_rec_sz(t)) == 0);

    sect_types_dtor(types);
    type_dtor(t);
    free(zr_itm);
    free(rd_itm);
    free(zr_rec);
    free(rd_rec);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

Status SectionTypes_DeleteTypeWithAttributesInner_DeleteAndShiftPtrs()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_types *types = sect_types_new();
    sect_types_ctor(types, 0, file);

    TYPE_INIT(t, "V");

    ATR_INIT(bool_atr, "bool", TYPE_BOOL);
    type_add_atr(t, bool_atr);
    ATR_INIT(int32_atr, "int32", TYPE_INT32);
    type_add_atr(t, int32_atr);
    ATR_INIT(str_atr, "str", TYPE_STRING);
    type_add_atr(t, str_atr);
    ATR_INIT(float_atr, "float", TYPE_FLOAT);
    type_add_atr(t, float_atr);

    TYPE_INIT(extra, "K");

    Sectoff wrt_adr;
    sect_types_write(types, t, &wrt_adr);

    Sectoff K_wrt_adr;
    sect_types_write(types, extra, &K_wrt_adr);

    sect_types_delete(types, wrt_adr);

    assert(types->header.free_space == SECTION_SIZE - sizeof(Sect_head_entity) - type_ph_sz(t) - type_ph_sz(extra));
    assert(types->header.next_ptr == 0); // Next section is undefined
    assert(types->header.lst_itm_ptr == sizeof(Sect_head_entity) + type_itm_sz(t) + type_itm_sz(extra));
    assert(types->header.fst_rec_ptr == SECTION_SIZE - type_rec_sz(t) - type_rec_sz(extra));
    assert(types->header.file_offset == 0);

    Sect_head_entity *head_entity = my_malloc(Sect_head_entity);
    sect_head_read((Sect_head *)types, 0, sizeof(Sect_head_entity), head_entity);
    assert(head_entity->free_space == SECTION_SIZE - sizeof(Sect_head_entity) - type_ph_sz(t) - type_ph_sz(extra));
    assert(head_entity->next_ptr == 0); // Next section is undefined
    assert(head_entity->lst_itm_ptr == sizeof(Sect_head_entity) + type_itm_sz(t) + type_itm_sz(extra));
    assert(head_entity->fst_rec_ptr == SECTION_SIZE - type_rec_sz(t) - type_rec_sz(extra));
    free(head_entity);

    char *zr_itm = calloc(type_itm_sz(t), sizeof(char));
    char *rd_itm = my_malloc_array(char, type_itm_sz(t));
    sect_head_read((Sect_head *)types, wrt_adr, type_itm_sz(t), rd_itm);
    assert(memcmp(rd_itm, zr_itm, type_itm_sz(t)) == 0);

    char *zr_rec = calloc(type_rec_sz(t), sizeof(char));
    char *rd_rec = my_malloc_array(char, type_rec_sz(t));
    sect_head_read((Sect_head *)types, SECTION_SIZE - type_rec_sz(t), type_rec_sz(t), rd_itm);
    assert(memcmp(rd_itm, zr_itm, type_rec_sz(t)) == 0);

    sect_types_dtor(types);
    type_dtor(t);
    type_dtor(extra);
    free(zr_itm);
    free(rd_itm);
    free(zr_rec);
    free(rd_rec);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_types_delete()
{
    assert(SectionTypes_DeleteTypeWithoutAttributesOnBorder_DeleteAndShiftPtrs() == OK);
    assert(SectionTypes_DeleteInnerTypeWithoutAttributes_Delete() == OK);
    assert(SectionTypes_DeleteInnerTypeWithoutAttributesAndAfterBorder_DeleteAllAndShiftPtrs() == OK);
    assert(SectionTypes_DeleteTypeWithAttributesOnBorder_DeleteAndShiftPtrs() == OK);
    assert(SectionTypes_DeleteTypeWithAttributesInner_DeleteAndShiftPtrs() == OK);
}