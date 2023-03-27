#include "tests/test.h"

#include "section/extents.h"
#include "section/extents_p.h"

static const char *test_file_name = "test.bin";

static Status SectionExtents_DefaultCtor_InvokeHeaderCtor()
{
    int START_ADR = 32;

    FILE *file = fopen(test_file_name, "w+");

    Sect_ext *extents = sect_ext_new();
    sect_ext_ctor(extents, START_ADR, file);
    assert(extents->header.free_space == (SECTION_SIZE - sizeof(Sect_head_entity)));
    assert(extents->header.next_ptr == 0); // Next section is undefined
    assert(extents->header.lst_itm_ptr == sizeof(Sect_head_entity));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE);
    assert(extents->header.file_offset == START_ADR);

    Sect_head_entity *head_entity = my_malloc(Sect_head_entity);
    sect_head_read(&extents->header, 0, sizeof(Sect_head_entity), head_entity);
    assert(head_entity->free_space == (SECTION_SIZE - sizeof(Sect_head_entity)));
    assert(head_entity->next_ptr == 0); // Next section is undefined
    assert(head_entity->lst_itm_ptr == sizeof(Sect_head_entity));
    assert(head_entity->fst_rec_ptr == SECTION_SIZE);

    sect_ext_dtor(extents);
    free(head_entity);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static Status SectionExtents_WriteTypedJsonValue_Successful(Json *j)
{
    const int START_ADR = 32;

    FILE *file = fopen(test_file_name, "w+");

    Sect_ext *extents = sect_ext_new();
    sect_ext_ctor(extents, START_ADR, file);

    Sectoff save_json_addr;
    ENTITY_INIT(ent, j, 500, 200, 300, 200);
    Status write_Status = sect_ext_write(extents, j->key, json_val_ptr(j), json_val_size(j), ent, &save_json_addr);
    assert(save_json_addr == sizeof(Sect_head_entity));

    assert(extents->header.free_space == SECTION_SIZE - sizeof(Sect_head_entity) - entity_ph_size(ent));
    assert(extents->header.next_ptr == 0); // Next section is undefined
    assert(extents->header.lst_itm_ptr == sizeof(Sect_head_entity) + entity_itm_size(ent));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE - entity_rec_sz(ent));
    assert(extents->header.file_offset == START_ADR);

    Sect_head_entity *head = my_malloc(Sect_head_entity);
    sect_head_read(&extents->header, 0, sizeof(Sect_head_entity), head);
    assert(head->free_space == SECTION_SIZE - sizeof(Sect_head_entity) - entity_ph_size(ent));
    assert(head->next_ptr == 0); // Next section is undefined
    assert(head->lst_itm_ptr == sizeof(Sect_head_entity) + entity_itm_size(ent));
    assert(head->fst_rec_ptr == SECTION_SIZE - entity_rec_sz(ent));

    Entity *rd_ent = entity_new();
    sect_ext_rd_itm(extents, save_json_addr, rd_ent);
    assert(rd_ent->key_ptr == SECTION_SIZE - string_get_size(j->key));
    assert(rd_ent->key_size == string_get_size(j->key));
    assert(rd_ent->val_ptr == rd_ent->key_ptr - json_val_size(j));
    assert(rd_ent->val_size == json_val_size(j));
    assert(rd_ent->fam_addr.dad_ptr == 500);
    assert(rd_ent->fam_addr.bro_ptr == 200);
    assert(rd_ent->fam_addr.son_ptr == 300);
    assert(rd_ent->type == ent->type);
    assert(rd_ent->init_rec_sz == ent->init_rec_sz);
    assert(rd_ent->type_ptr == 200);

    char *key_val = my_malloc_array(char, rd_ent->key_size);
    sect_head_read(&extents->header, rd_ent->key_ptr, rd_ent->key_size, key_val);
    assert(strncmp(key_val, j->key->val, j->key->cnt) == 0);
    free(key_val);

    void *val_val = my_malloc_array(char, rd_ent->val_size);
    sect_head_read(&extents->header, rd_ent->val_ptr, rd_ent->val_size, val_val);
    assert(strncmp(val_val, json_val_ptr(j), json_val_size(j)) == 0);
    free(val_val);

    entity_dtor(ent);
    entity_dtor(rd_ent);

    free(head);
    sect_ext_dtor(extents);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return write_Status;
}

static Status SectionExtents_WriteStringJsonValueWithNotEnoughSpace_Failed()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_ext *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    String *str = string_new();
    string_ctor(str, "hello world!", 12);

    JSON_VALUE_INIT(TYPE_STRING, json, "value", str);
    ENTITY_INIT(entity, json, 500, 0, 0, 200);

    extents->header.free_space = 5;

    Fileoff save_json_addr;
    Status write_Status = sect_ext_write(extents, json->key, json_val_ptr(json), json_val_size(json), entity, &save_json_addr);

    assert(extents->header.free_space == 5);
    assert(extents->header.next_ptr == 0); // Next section is undefined
    assert(extents->header.lst_itm_ptr == sizeof(Sect_head_entity));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE);
    assert(extents->header.file_offset == 0);

    Sect_head_entity *head_entity = my_malloc(Sect_head_entity);
    sect_head_read(&extents->header, 0, sizeof(Sect_head_entity), head_entity);
    assert(head_entity->free_space == (SECTION_SIZE - sizeof(Sect_head_entity)));
    assert(head_entity->next_ptr == 0); // Next section is undefined
    assert(head_entity->lst_itm_ptr == sizeof(Sect_head_entity));
    assert(head_entity->fst_rec_ptr == SECTION_SIZE);

    json_dtor(json);

    entity_dtor(entity);

    sect_ext_dtor(extents);
    free(head_entity);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return write_Status;
}

void test_extents_write()
{
    assert(SectionExtents_DefaultCtor_InvokeHeaderCtor() == OK);

    JSON_VALUE_INIT(TYPE_INT32, int32_j, "int32", 5);
    assert(SectionExtents_WriteTypedJsonValue_Successful(int32_j) == OK);
    JSON_VALUE_INIT(TYPE_FLOAT, float_j, "float", 5.5);
    assert(SectionExtents_WriteTypedJsonValue_Successful(float_j) == OK);
    JSON_VALUE_INIT(TYPE_BOOL, bool_j, "bool", true);
    assert(SectionExtents_WriteTypedJsonValue_Successful(bool_j) == OK);
    STR_INIT(str, "kekeke");
    JSON_VALUE_INIT(TYPE_STRING, string_j, "string", str);
    assert(SectionExtents_WriteTypedJsonValue_Successful(string_j) == OK);
    JSON_VALUE_INIT(TYPE_OBJECT, obj_j, "object", NULL);
    assert(SectionExtents_WriteTypedJsonValue_Successful(obj_j) == OK);

    json_dtor(int32_j);
    json_dtor(float_j);
    json_dtor(bool_j);
    json_dtor(string_j);
    json_dtor(obj_j);
    
    assert(SectionExtents_WriteStringJsonValueWithNotEnoughSpace_Failed() == FAILED);
}
