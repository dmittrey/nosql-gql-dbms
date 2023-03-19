#include "tests/test.h"

#include "section/types.h"
#include "section/types_p.h"

static const char *test_file_name = "test.bin";

/*
1) Найти тип в пустой секции
2) Найти тип в секции с одним типом
3) Найти тип в секции с несколькими разными типами
*/

Status SectionTypes_FindFromEmptySection_FindNothing()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_types *types = sect_types_new();
    sect_types_ctor(types, 0, file);

    Sectoff t_adr;
    Type* t = type_new();
    STR_INIT(t_name, "example");
    sect_types_find(types, t_name, t, &t_adr);

    assert(t->attr_list == NULL);
    assert(t->name == NULL);
    assert(t->next == NULL);

    sect_types_dtor(types);
    type_dtor(t);
    string_dtor(t_name);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

Status SectionTypes_FindOneTypeFromOneEmpty_FindNothing()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_types *types = sect_types_new();
    sect_types_ctor(types, 0, file);

    TYPE_INIT(V_type, "V");

    Sectoff wrt_adr;
    sect_types_write(types, V_type, &wrt_adr);

    Sectoff t_adr;
    Type *t = type_new();
    STR_INIT(t_name, "V");
    sect_types_find(types, t_name, t, &t_adr);

    assert(type_cmp(t, V_type) == 0);

    sect_types_dtor(types);
    type_dtor(V_type);
    type_dtor(t);
    string_dtor(t_name);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

Status SectionTypes_FindOneTypeFromSeveral_FindNothing()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_types *types = sect_types_new();
    sect_types_ctor(types, 0, file);

    Sectoff wrt_adr;
    TYPE_INIT(V_type, "V");
    sect_types_write(types, V_type, &wrt_adr);
    TYPE_INIT(T_type, "T");
    sect_types_write(types, T_type, &wrt_adr);
    TYPE_INIT(K_type, "K");
    sect_types_write(types, K_type, &wrt_adr);

    Sectoff t_adr;
    Type *t = type_new();
    STR_INIT(t_name, "T");
    sect_types_find(types, t_name, t, &t_adr);

    assert(type_cmp(t, T_type) == 0);

    sect_types_dtor(types);
    type_dtor(V_type);
    type_dtor(K_type);
    type_dtor(T_type);
    type_dtor(t);
    string_dtor(t_name);

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