#include <assert.h>
#include <string.h>

#include "memory/section/types.h"
#include "memory/type/type.h"

#include "physical/section/types.h"
#include "physical/type/type.h"

static const char *test_file_name = "test.bin";

/*
1) Найти тип в пустой секции
2) Найти тип в секции с одним типом
3) Найти тип в секции с несколькими разными типами
*/

status_t SectionTypes_FindFromEmptySection_FindNothing()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_type_t *types = sect_type_new();
    sect_type_ctor(types, 0, file);

    list_type_t *t_list = list_type_t_new();

    STR_INIT(q_name, "example");
    sect_type_find(types, q_name, t_list);

    assert(t_list->head == NULL);
    assert(t_list->tail == NULL);
    assert(t_list->count == 0);

    list_type_t_dtor(t_list);
    string_dtor(q_name);

    sect_type_dtor(types);
    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t SectionTypes_FindOneTypeFromOneEmpty_FindNothing()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_type_t *types = sect_type_new();
    sect_type_ctor(types, 0, file);

    TYPE_INIT(wr_type, "V");
    sectoff_t wrt_adr;
    sect_type_write(types, wr_type, &wrt_adr);

    type_t *t = type_new();

    STR_INIT(q_name, "V");
    sect_type_find(types, q_name, t);

    assert(string_cmp(t->name, q_name) == 0);
    assert(t->attr_list->count == 0);
    assert(t->foff_ptr == sizeof(sect_head_entity_t));

    type_dtor(wr_type);
    type_dtor(t);

    string_dtor(q_name);

    sect_type_dtor(types);
    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t SectionTypes_FindOneTypeFromSeveral_FindNothing()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_type_t *types = sect_type_new();
    sect_type_ctor(types, 0, file);

    sectoff_t wrt_adr;
    TYPE_INIT(V_type, "V");
    sect_type_write(types, V_type, &wrt_adr);
    TYPE_INIT(T_type, "T");
    sect_type_write(types, T_type, &wrt_adr);
    TYPE_INIT(K_type, "K");
    sect_type_write(types, K_type, &wrt_adr);

    type_t *t = type_new();

    STR_INIT(q_name, "K");
    sect_type_find(types, q_name, t);

    assert(string_cmp(t->name, q_name) == 0);
    assert(t->attr_list->count == 0);
    assert(t->foff_ptr == sizeof(sect_head_entity_t) + 2 * sizeof(type_entity_t));

    type_dtor(V_type);
    type_dtor(K_type);
    type_dtor(T_type);
    type_dtor(t);
    
    string_dtor(q_name);

    sect_type_dtor(types);
    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_types_find()
{
    assert(SectionTypes_FindFromEmptySection_FindNothing() == OK);
    assert(SectionTypes_FindOneTypeFromOneEmpty_FindNothing() == OK);
    assert(SectionTypes_FindOneTypeFromSeveral_FindNothing() == OK);
}