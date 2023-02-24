#include <assert.h>
#include <string.h>

#include "memory/section/extents.h"

#include "physical/json/entity.h"
#include "physical/section/extents.h"
/*
1) Посмотреть обновление граничного элементы
2) Посмотреть обновление элемента внутри и хватает места
3) посмотреть обновление элемента когда не хватает места вставить  
4) Поменять с INT32 на STRING
*/

static const char *test_file_name = "test.bin";

static status_t SectionExtents_UpdateBoundaryElement_ShiftPtrsAndUpdateFreeSpaceAndUpdate()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, json, "value", 5);

    fileoff_t save_json_addr;
    DO_OR_FAIL(sect_ext_write(extents, json, 500, 200, 300, &save_json_addr));

    string_t *str = string_new();
    string_ctor(str, "more_than_sixteen_elements_string", 33);
    JSON_VALUE_INIT(TYPE_STRING, updated_json, "value", str);

    DO_OR_FAIL(sect_ext_update(extents, save_json_addr, updated_json));

    json_t *r_json = json_new();
    entity_t *r_entity = entity_new();
    DO_OR_FAIL(sect_ext_read(extents, save_json_addr, r_entity, r_json));

    assert(extents->header.free_space == (SECTION_SIZE - sizeof(sect_head_entity_t) - sizeof(entity_t) - string_get_size(updated_json->key) - string_get_size(updated_json->value.string_val)));
    assert(extents->header.next_ptr == 0);
    assert(extents->header.lst_itm_ptr == sizeof(sect_head_entity_t) + sizeof(entity_t));
    assert(extents->header.fst_rec_ptr == r_entity->val_size);

    assert(r_entity->key_ptr == SECTION_SIZE - string_get_size(updated_json->key));
    assert(r_entity->key_size == string_get_size(updated_json->key));
    assert(r_entity->val_ptr == SECTION_SIZE - string_get_size(updated_json->key) - string_get_size(updated_json->value.string_val));
    assert(r_entity->val_size == string_get_size(updated_json->value.string_val));
    assert(r_entity->dad_ptr == 500);
    assert(r_entity->bro_ptr == 200);
    assert(r_entity->son_ptr == 300);
    assert(r_entity->type == TYPE_STRING);

    assert(r_json->key->cnt == updated_json->key->cnt);
    assert(strncmp(r_json->key->val, updated_json->key->val, r_json->key->cnt) == 0);
    assert(r_json->value.string_val->cnt == updated_json->value.string_val->cnt);
    assert(strncmp(r_json->value.string_val->val, updated_json->value.string_val->val, r_json->value.string_val->cnt) == 0);
    assert(r_json->type == TYPE_STRING);

    return OK;
}

static status_t SectionExtents_UpdateInsideElement_Update()
{

}

static status_t SectionExtents_UpdateInsideElementWithNotEnoughSpace_ReturnFailed()
{

}

void test_extents_update()
{
    assert(SectionExtents_UpdateBoundaryElement_ShiftPtrsAndUpdateFreeSpaceAndUpdate() == OK);
    assert(SectionExtents_UpdateInsideElement_Update() == OK);
    assert(SectionExtents_UpdateInsideElementWithNotEnoughSpace_ReturnFailed() == OK);
}
