#include <assert.h>
#include <string.h>

#include "memory/section/types.h"
#include "memory/type/type.h"

#include "physical/section/types.h"
#include "physical/type/type.h"

static const char *test_file_name = "test.bin";

/*
1) Запись конструктора
2) Запись пустого типа
3) Запись типа состоящего из всех типов атрибутов
4) Запись типа в недостаточное место
*/

status_t SectionTypes_Ctor_InvokeHeaderCtor()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_type_t *types = sect_type_new();
    sect_type_ctor(types, 0, file);

    assert(types->header.free_space == (SECTION_SIZE - sizeof(sect_head_entity_t)));
    assert(types->header.next_ptr == 0); // Next section is undefined
    assert(types->header.lst_itm_ptr == sizeof(sect_head_entity_t));
    assert(types->header.fst_rec_ptr == SECTION_SIZE);

    sect_head_entity_t *header = sect_head_entity_new();
    FREAD_OR_FAIL(header, sizeof(sect_head_entity_t), types->header.filp);

    assert(header->free_space == (SECTION_SIZE - sizeof(sect_head_entity_t)));
    assert(header->next_ptr == 0); // Next section is undefined
    assert(header->lst_itm_ptr == sizeof(sect_head_entity_t));
    assert(header->fst_rec_ptr == SECTION_SIZE);

    sect_type_dtor(types);
    sect_head_entity_dtor(header);
    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t SectionTypes_WriteEmptyType_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_type_t *types = sect_type_new();
    sect_type_ctor(types, 0, file);

    type_t *wr_type = type_new();
    STR_INIT(type_name, "V");
    list_attr_t *wr_attr_list = list_attr_t_new();

    type_ctor(wr_type, type_name, wr_attr_list);
    sect_type_write(types, wr_type);

    // Assert ptrs shifts
    assert(types->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t) - sizeof(type_entity_t) - wr_type->name->cnt);
    assert(types->header.lst_itm_ptr == sizeof(sect_head_entity_t) + sizeof(type_entity_t));
    assert(types->header.fst_rec_ptr == SECTION_SIZE - wr_type->name->cnt * sizeof(char));

    type_entity_t *rd_type_ent = type_entity_new();
    sect_type_rd_ent(types, sizeof(sect_head_entity_t), rd_type_ent);

    assert(rd_type_ent->attr_cnt == wr_type->attr_list->count);
    assert(rd_type_ent->name_ptr == types->header.fst_rec_ptr);
    assert(rd_type_ent->name_size == wr_type->name->cnt);

    type_dtor(wr_type);

    type_entity_dtor(rd_type_ent);

    sect_type_dtor(types);
    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t SectionTypes_WriteAllAttributesType_Successful()
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
    sect_type_write(types, wr_type);

    // Assert ptrs shifts
    assert(types->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t) - sizeof(type_entity_t) - 1 * sizeof(char) - 4 * sizeof(attr_entity_t) - 4 * 4 * sizeof(char));
    assert(types->header.lst_itm_ptr == sizeof(sect_head_entity_t) + sizeof(type_entity_t) + 4 * sizeof(attr_entity_t));
    assert(types->header.fst_rec_ptr == SECTION_SIZE - 1 * sizeof(char) - 4 * 4 * sizeof(char));

    type_entity_t *rd_type_ent = type_entity_new();
    sect_type_rd_ent(types, sizeof(sect_head_entity_t), rd_type_ent);

    assert(rd_type_ent->attr_cnt == wr_type->attr_list->count);
    assert(rd_type_ent->name_ptr == SECTION_SIZE - 1 * sizeof(char));
    assert(rd_type_ent->name_size == wr_type->name->cnt);

    attr_entity_t *rd_attr_ent = attr_entity_new();
    sect_type_rd_atr(types, sizeof(sect_head_entity_t) + sizeof(type_entity_t), rd_attr_ent);
    assert(rd_attr_ent->name_size == 4);
    assert(rd_attr_ent->name_ptr == SECTION_SIZE - 1 * sizeof(char) - 4 * sizeof(char));
    assert(rd_attr_ent->attr_type == TYPE_BOOL);

    sect_type_rd_atr(types, sizeof(sect_head_entity_t) + sizeof(type_entity_t) + sizeof(attr_entity_t), rd_attr_ent);
    assert(rd_attr_ent->name_size == 4);
    assert(rd_attr_ent->name_ptr == SECTION_SIZE - 1 * sizeof(char) - 2 * 4 * sizeof(char));
    assert(rd_attr_ent->attr_type == TYPE_INT32);

    sect_type_rd_atr(types, sizeof(sect_head_entity_t) + sizeof(type_entity_t) + 2 * sizeof(attr_entity_t), rd_attr_ent);
    assert(rd_attr_ent->name_size == 4);
    assert(rd_attr_ent->name_ptr == SECTION_SIZE - 1 * sizeof(char) - 3 * 4 * sizeof(char));
    assert(rd_attr_ent->attr_type == TYPE_STRING);

    sect_type_rd_atr(types, sizeof(sect_head_entity_t) + sizeof(type_entity_t) + 3 * sizeof(attr_entity_t), rd_attr_ent);
    assert(rd_attr_ent->name_size == 4);
    assert(rd_attr_ent->name_ptr == SECTION_SIZE - 1 * sizeof(char) - 4 * 4 * sizeof(char));
    assert(rd_attr_ent->attr_type == TYPE_FLOAT);

    type_dtor(wr_type);

    type_entity_dtor(rd_type_ent);

    sect_type_dtor(types);
    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t SectionTypes_WriteAllAttributesTypeWithNotEnoughSpaceSection_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_type_t *types = sect_type_new();
    sect_type_ctor(types, 0, file);

    sect_head_shift_lst_itm_ptr(&types->header, 8100);

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
    status_t wr_status = sect_type_write(types, wr_type);

    type_dtor(wr_type);

    sect_type_dtor(types);
    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return wr_status;
}

void test_types_write()
{
    assert(SectionTypes_Ctor_InvokeHeaderCtor() == OK);
    assert(SectionTypes_WriteEmptyType_Successful() == OK);
    assert(SectionTypes_WriteAllAttributesType_Successful() == OK);
    assert(SectionTypes_WriteAllAttributesTypeWithNotEnoughSpaceSection_Successful() == FAILED);
}