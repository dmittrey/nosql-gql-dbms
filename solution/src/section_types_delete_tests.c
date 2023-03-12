#include <assert.h>
#include <string.h>

#include "memory/section/types.h"
#include "memory/type/type.h"

#include "physical/section/types.h"
#include "physical/type/type.h"

static const char *test_file_name = "test.bin";

/*
1) Удаление типа без атрибутов на границе
2) Удаление типа без атрибутов не на границе
3) Удаление внутри а потом на границе(ptrs promotion check)
4) Удаление типа с атрибутом на границе
*/

status_t SectionTypes_DeleteTypeWithoutAttributesOnBorder_DeleteAndShiftPtrs()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_type_t *types = sect_type_new();
    sect_type_ctor(types, 0, file);

    TYPE_INIT(wr_type, "V");
    sectoff_t wrt_adr;
    sect_type_write(types, wr_type, &wrt_adr);

    sect_type_delete(types, wrt_adr);

    assert(types->header.lst_itm_ptr == sizeof(sect_head_entity_t));
    assert(types->header.fst_rec_ptr == SECTION_SIZE);
    assert(types->header.free_space == types->header.fst_rec_ptr - types->header.lst_itm_ptr);

    type_entity_t *zero_ent = type_entity_new();
    type_entity_t *temp_ent = type_entity_new();
    sect_type_rd_ent(types, sizeof(sect_head_entity_t), temp_ent);

    assert(memcmp(zero_ent, temp_ent, sizeof(type_entity_t)) == 0);

    type_dtor(wr_type);

    type_entity_dtor(zero_ent);
    type_entity_dtor(temp_ent);

    sect_type_dtor(types);
    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t SectionTypes_DeleteInnerTypeWithoutAttributes_Delete()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_type_t *types = sect_type_new();
    sect_type_ctor(types, 0, file);

    TYPE_INIT(V_type, "V");
    sectoff_t wrt_adr;
    sect_type_write(types, V_type, &wrt_adr);

    TYPE_INIT(K_type, "K");
    sect_type_write(types, K_type, &wrt_adr);

    sect_type_delete(types, sizeof(sect_head_entity_t));

    assert(types->header.lst_itm_ptr == sizeof(sect_head_entity_t) + 2 * sizeof(type_entity_t));
    assert(types->header.fst_rec_ptr == SECTION_SIZE - 2 * sizeof(char));
    assert(types->header.free_space == types->header.fst_rec_ptr - types->header.lst_itm_ptr);

    type_entity_t *zero_ent = type_entity_new();
    type_entity_t *temp_ent = type_entity_new();

    sect_type_rd_ent(types, sizeof(sect_head_entity_t), temp_ent);
    assert(memcmp(zero_ent, temp_ent, sizeof(type_entity_t)) == 0);

    sect_type_rd_ent(types, sizeof(sect_head_entity_t) + sizeof(type_entity_t), temp_ent);
    assert(memcmp(zero_ent, temp_ent, sizeof(type_entity_t)) != 0);

    type_dtor(V_type);
    type_dtor(K_type);

    type_entity_dtor(zero_ent);
    type_entity_dtor(temp_ent);

    sect_type_dtor(types);
    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t SectionTypes_DeleteInnerTypeWithoutAttributesAndAfterBorder_DeleteAllAndShiftPtrs()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_type_t *types = sect_type_new();
    sect_type_ctor(types, 0, file);

    TYPE_INIT(V_type, "V");
    sectoff_t wrt_adr;
    sect_type_write(types, V_type, &wrt_adr);

    TYPE_INIT(K_type, "K");
    sect_type_write(types, K_type, &wrt_adr);

    sect_type_delete(types, sizeof(sect_head_entity_t));
    sect_type_delete(types, sizeof(sect_head_entity_t) + sizeof(type_entity_t));

    assert(types->header.lst_itm_ptr == sizeof(sect_head_entity_t));
    assert(types->header.fst_rec_ptr == SECTION_SIZE);
    assert(types->header.free_space == types->header.fst_rec_ptr - types->header.lst_itm_ptr);

    type_entity_t *zero_ent = type_entity_new();
    type_entity_t *temp_ent = type_entity_new();

    sect_type_rd_ent(types, sizeof(sect_head_entity_t), temp_ent);
    assert(memcmp(zero_ent, temp_ent, sizeof(type_entity_t)) == 0);

    sect_type_rd_ent(types, sizeof(sect_head_entity_t) + sizeof(type_entity_t), temp_ent);
    assert(memcmp(zero_ent, temp_ent, sizeof(type_entity_t)) == 0);

    type_dtor(V_type);
    type_dtor(K_type);

    type_entity_dtor(zero_ent);
    type_entity_dtor(temp_ent);

    sect_type_dtor(types);
    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t SectionTypes_DeleteTypeWithAttributesOnBorder_DeleteAndShiftPtrs()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_type_t *types = sect_type_new();
    sect_type_ctor(types, 0, file);

    type_t *wr_type = type_new();
    STR_INIT(type_name, "V");
    list_attr_t *wr_attr_list = list_attr_t_new();

    attr_t *bool_attr = attr_new();
    STR_INIT(bool_attr_name, "attr");
    attr_ctor(bool_attr, bool_attr_name, TYPE_BOOL);
    list_attr_t_add(wr_attr_list, bool_attr);

    attr_t *int_attr = attr_new();
    STR_INIT(int_attr_name, "attr");
    attr_ctor(int_attr, int_attr_name, TYPE_INT32);
    list_attr_t_add(wr_attr_list, int_attr);

    attr_t *str_attr = attr_new();
    STR_INIT(str_attr_name, "attr");
    attr_ctor(str_attr, str_attr_name, TYPE_STRING);
    list_attr_t_add(wr_attr_list, str_attr);

    attr_t *float_attr = attr_new();
    STR_INIT(float_attr_name, "attr");
    attr_ctor(float_attr, float_attr_name, TYPE_FLOAT);
    list_attr_t_add(wr_attr_list, float_attr);

    type_ctor(wr_type, type_name, wr_attr_list);
    sectoff_t wrt_adr;
    sect_type_write(types, wr_type, &wrt_adr);

    sect_type_delete(types, sizeof(sect_head_entity_t));

    assert(types->header.lst_itm_ptr == sizeof(sect_head_entity_t));
    assert(types->header.fst_rec_ptr == SECTION_SIZE);
    assert(types->header.free_space == types->header.fst_rec_ptr - types->header.lst_itm_ptr);

    type_entity_t *zero_ent = type_entity_new();
    type_entity_t *temp_ent = type_entity_new();
    sect_type_rd_ent(types, sizeof(sect_head_entity_t), temp_ent);

    assert(memcmp(zero_ent, temp_ent, sizeof(type_entity_t)) == 0);

    type_dtor(wr_type);

    type_entity_dtor(zero_ent);
    type_entity_dtor(temp_ent);

    sect_type_dtor(types);
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
}