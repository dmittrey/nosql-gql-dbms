#include <assert.h>
#include <string.h>

#include "memory/section/extents.h"

#include "physical/json/entity.h"
#include "physical/section/extents.h"
/*
1) Удалить из середины
2) Удалить граничный и посмотреть ссылки
*/

static const char *test_file_name = "test.bin";

static status_t SectionExtents_DeleteIntBoundaryElement_ShiftPtrs()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, json, "value", 5);
    ENTITY_INIT(entity, json, 500, 200, 300);

    fileoff_t save_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, json, entity, &save_json_addr));

    DO_OR_FAIL(sect_ext_delete(extents, save_json_addr));

    assert(extents->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t));
    assert(extents->header.next_ptr == 0);
    assert(extents->header.lst_itm_ptr == sizeof(sect_head_entity_t));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE);

    json_dtor(json);

    entity_dtor(entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t SectionExtents_DeleteStringBoundaryElement_ShiftPtrs()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    string_t *str = string_new();
    string_ctor(str, "more_than_sixteen_elements_string", 33);
    JSON_VALUE_INIT(TYPE_STRING, json, "value", str);
    ENTITY_INIT(entity, json, 500, 200, 300);

    fileoff_t save_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, json, entity, &save_json_addr));

    DO_OR_FAIL(sect_ext_delete(extents, save_json_addr));

    assert(extents->header.free_space == SECTION_SIZE - sizeof(sect_head_entity_t));
    assert(extents->header.next_ptr == 0);
    assert(extents->header.lst_itm_ptr == sizeof(sect_head_entity_t));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE);

    json_dtor(json);

    entity_dtor(entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t SectionExtents_UpdateInsideElement_Nothing()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, first_json, "value", 5);
    ENTITY_INIT(f_entity, first_json, 500, 0, 0);
    JSON_VALUE_INIT(TYPE_INT32, second_json, "value", 6);
    ENTITY_INIT(s_entity, second_json, 500, 0, 0);

    fileoff_t first_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, first_json, f_entity, &first_json_addr));
    fileoff_t second_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, second_json, s_entity, &second_json_addr));

    DO_OR_FAIL(sect_ext_delete(extents, first_json_addr));

    assert(extents->header.free_space == (SECTION_SIZE - sizeof(sect_head_entity_t) - 2 * sizeof(entity_t) - string_get_size(first_json->key) - string_get_size(second_json->key) - 2 * sizeof(first_json->value)));
    assert(extents->header.next_ptr == 0);
    assert(extents->header.lst_itm_ptr == sizeof(sect_head_entity_t) + 2 * sizeof(entity_t));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE - string_get_size(first_json->key) - string_get_size(second_json->key) - 2 * sizeof(first_json->value));

    json_dtor(first_json);
    json_dtor(second_json);

    entity_dtor(f_entity);
    entity_dtor(s_entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_extents_delete()
{
    assert(SectionExtents_DeleteIntBoundaryElement_ShiftPtrs() == OK);
    assert(SectionExtents_DeleteStringBoundaryElement_ShiftPtrs() == OK);
    assert(SectionExtents_UpdateInsideElement_Nothing() == OK);
}