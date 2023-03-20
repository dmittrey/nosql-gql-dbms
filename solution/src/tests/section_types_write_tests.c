#include "tests/test.h"

#include "section/types.h"
#include "section/types_p.h"

static const char *test_file_name = "test.bin";

/*
1) Запись конструктора
2) Запись пустого типа
3) Запись типа состоящего из всех типов атрибутов
4) Запись типа в недостаточное место
*/
static Status SectionTypes_Ctor_InvokeHeaderCtor()
{
    int START_ADR = 32;

    FILE *file = fopen(test_file_name, "w+");

    Sect_types *types = sect_types_new();
    sect_types_ctor(types, START_ADR, file);
    assert(types->header.free_space == (SECTION_SIZE - sizeof(Sect_head_entity)));
    assert(types->header.next_ptr == 0); // Next section is undefined
    assert(types->header.lst_itm_ptr == sizeof(Sect_head_entity));
    assert(types->header.fst_rec_ptr == SECTION_SIZE);
    assert(types->header.file_offset == START_ADR);

    Sect_head_entity *head_entity = my_malloc(Sect_head_entity);
    sect_head_read((Sect_head *)types, 0, sizeof(Sect_head_entity), head_entity);
    assert(head_entity->free_space == (SECTION_SIZE - sizeof(Sect_head_entity)));
    assert(head_entity->next_ptr == 0); // Next section is undefined
    assert(head_entity->lst_itm_ptr == sizeof(Sect_head_entity));
    assert(head_entity->fst_rec_ptr == SECTION_SIZE);
    free(head_entity);

    sect_types_dtor(types);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static Status SectionTypes_WriteEmptyType_Successful()
{
    int START_ADR = 32;

    FILE *file = fopen(test_file_name, "w+");

    Sect_types *types = sect_types_new();
    sect_types_ctor(types, START_ADR, file);

    TYPE_INIT(wr_type, "V");

    Sectoff wrt_adr;
    sect_types_write(types, wr_type, &wrt_adr);

    assert(types->header.free_space == SECTION_SIZE - sizeof(Sect_head_entity) - type_ph_sz(wr_type));
    assert(types->header.next_ptr == 0); // Next section is undefined
    assert(types->header.lst_itm_ptr == sizeof(Sect_head_entity) + sizeof(Type_entity));
    assert(types->header.fst_rec_ptr == SECTION_SIZE - wr_type->name->cnt);
    assert(types->header.file_offset == START_ADR);

    Sect_head_entity *head_entity = my_malloc(Sect_head_entity);
    sect_head_read((Sect_head *)types, 0, sizeof(Sect_head_entity), head_entity);
    assert(head_entity->free_space == SECTION_SIZE - sizeof(Sect_head_entity) - type_ph_sz(wr_type));
    assert(head_entity->next_ptr == 0); // Next section is undefined
    assert(head_entity->lst_itm_ptr == sizeof(Sect_head_entity) + sizeof(Type_entity));
    assert(head_entity->fst_rec_ptr == SECTION_SIZE - wr_type->name->cnt);
    free(head_entity);

    Type_entity *t_ent = type_entity_new();
    sect_head_read((Sect_head *)types, wrt_adr, sizeof(Type_entity), t_ent);
    assert(t_ent->attr_cnt == wr_type->attr_list->count);
    assert(t_ent->name_size == wr_type->name->cnt);
    assert(t_ent->name_ptr == SECTION_SIZE - wr_type->name->cnt);

    char *rec_data = my_malloc_array(char, wr_type->name->cnt);
    sect_head_read((Sect_head *)types, t_ent->name_ptr, wr_type->name->cnt, rec_data);
    assert(memcmp(rec_data, wr_type->name->val, wr_type->name->cnt) == 0);

    sect_types_dtor(types);
    type_dtor(wr_type);
    type_entity_dtor(t_ent);
    free(rec_data);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static Status SectionTypes_WriteAllAttributesType_Successful()
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
    assert(types->header.free_space == SECTION_SIZE - sizeof(Sect_head_entity) - type_ph_sz(t));
    assert(types->header.next_ptr == 0); // Next section is undefined
    assert(types->header.lst_itm_ptr == sizeof(Sect_head_entity) + type_itm_sz(t));
    assert(types->header.fst_rec_ptr == SECTION_SIZE - type_rec_sz(t));
    assert(types->header.file_offset == 0);

    Sect_head_entity *head_entity = my_malloc(Sect_head_entity);
    sect_head_read((Sect_head *)types, 0, sizeof(Sect_head_entity), head_entity);
    assert(head_entity->free_space == SECTION_SIZE - sizeof(Sect_head_entity) - type_ph_sz(t));
    assert(head_entity->next_ptr == 0); // Next section is undefined
    assert(head_entity->lst_itm_ptr == sizeof(Sect_head_entity) + type_itm_sz(t));
    assert(head_entity->fst_rec_ptr == SECTION_SIZE - type_rec_sz(t));
    free(head_entity);

    Type_entity *rd_type_ent = type_entity_new();
    sect_types_rd_type(types, wrt_adr, rd_type_ent);
    assert(rd_type_ent->attr_cnt == t->attr_list->count);
    assert(rd_type_ent->name_ptr == SECTION_SIZE - t->name->cnt);
    assert(rd_type_ent->name_size == t->name->cnt);

    Attr_entity *rd_attr_ent = attr_entity_new();
    sect_types_rd_atr(types, wrt_adr + sizeof(Type_entity), rd_attr_ent);
    assert(rd_attr_ent->name_size == bool_atr->name->cnt);
    assert(rd_attr_ent->name_ptr == rd_type_ent->name_ptr - bool_atr->name->cnt);
    assert(rd_attr_ent->attr_type == TYPE_BOOL);

    sect_types_rd_atr(types, wrt_adr + sizeof(Type_entity) + sizeof(Attr_entity), rd_attr_ent);
    assert(rd_attr_ent->name_size == int32_atr->name->cnt);
    assert(rd_attr_ent->name_ptr == rd_type_ent->name_ptr - bool_atr->name->cnt - int32_atr->name->cnt);
    assert(rd_attr_ent->attr_type == TYPE_INT32);

    sect_types_rd_atr(types, wrt_adr + sizeof(Type_entity) + 2 * sizeof(Attr_entity), rd_attr_ent);
    assert(rd_attr_ent->name_size == str_atr->name->cnt);
    assert(rd_attr_ent->name_ptr == rd_type_ent->name_ptr - bool_atr->name->cnt - int32_atr->name->cnt - str_atr->name->cnt);
    assert(rd_attr_ent->attr_type == TYPE_STRING);

    sect_types_rd_atr(types, wrt_adr + sizeof(Type_entity) + 3 * sizeof(Attr_entity), rd_attr_ent);
    assert(rd_attr_ent->name_size == float_atr->name->cnt);
    assert(rd_attr_ent->name_ptr == rd_type_ent->name_ptr - bool_atr->name->cnt - int32_atr->name->cnt - str_atr->name->cnt - float_atr->name->cnt);
    assert(rd_attr_ent->attr_type == TYPE_FLOAT);

    sect_types_dtor(types);
    type_dtor(t);
    type_entity_dtor(rd_type_ent);
    attr_entity_dtor(rd_attr_ent);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static Status SectionTypes_WriteAllAttributesTypeWithNotEnoughSpaceSection_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_types *types = sect_types_new();
    sect_types_ctor(types, 0, file);

    sect_head_shift_lip(&types->header, SECTION_SIZE - 92);

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
    Status wr_status = sect_types_write(types, t, &wrt_adr);

    type_dtor(t);

    sect_types_dtor(types);
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