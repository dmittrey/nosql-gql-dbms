#include <assert.h>

#include "physical/json/entity.h"

#include "physical/section/header.h"

#include "memory/section/extents_p.h"
#include "memory/section/extents.h"

static const char *test_file_name = "test.bin";

static status_t SectionExtents_DefaultCtor_InvokeHeaderCtor()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    assert(extents->header.free_space == (SECTION_SIZE - sizeof(sect_head_entity_t)));
    assert(extents->header.next_ptr == 0); // Next section is undefined
    assert(extents->header.lst_itm_ptr == sizeof(sect_head_entity_t));
    assert(extents->header.fst_rec_ptr == SECTION_SIZE);

    // sect_ext_dtor(extents);   // No make sense
    free(extents); // Make sense
    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static status_t SectionExtents_InvokeSync_InvokeHeaderSync()
{
    FILE *file = fopen(test_file_name, "w+");
    sect_ext_t *extents = sect_ext_new();

    extents->header.free_space = 5;
    extents->header.next_ptr = 6;
    extents->header.lst_itm_ptr = 7;
    extents->header.fst_rec_ptr = 8;
    extents->header.sect_off = 0;
    extents->header.filp = file;

    DO_OR_FAIL(sect_ext_sync(extents));

    sect_head_entity_t *header = my_malloc(sect_head_entity_t);
    FREAD_OR_FAIL(header, sizeof(sect_head_entity_t), file);

    assert(header->free_space == 5);
    assert(header->next_ptr == 6);
    assert(header->lst_itm_ptr == 7);
    assert(header->fst_rec_ptr == 8);

    free(header);
    sect_ext_dtor(extents);
    fclose(file);

    if (remove(test_file_name))
    {
        return FAILED;
    }

    return OK;
}

static status_t SectionExtents_WriteInt32JsonValue_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_INT32, json, "value", 5);
    ENTITY_INIT(entity, json, 500, 0, 0);

    fileoff_t save_json_addr;
    status_t write_status = sect_ext_write(extents, json, entity, &save_json_addr);
    assert(save_json_addr == sizeof(sect_head_entity_t));

    sect_head_entity_t *header = my_malloc(sect_head_entity_t);
    RA_FREAD_OR_FAIL(header, sizeof(sect_head_entity_t), 0, file);
    size_t a = sizeof(sect_head_entity_t);
    size_t b = sizeof(entity_t);
    size_t c = sizeof(json->value);
    size_t d = string_get_size(json->key);
    assert(header->free_space == (SECTION_SIZE - sizeof(sect_head_entity_t) - sizeof(entity_t) - sizeof(json->value) - string_get_size(json->key))); // 32 + 64 + 5 + 16
    assert(header->fst_rec_ptr == (SECTION_SIZE - sizeof(json->value) - string_get_size(json->key)));
    assert(header->lst_itm_ptr == sizeof(sect_head_entity_t) + sizeof(entity_t));

    entity_t *json_entity = entity_new();
    RA_FREAD_OR_FAIL(json_entity, sizeof(entity_t), save_json_addr, extents->header.filp);
    assert(json_entity->key_ptr == SECTION_SIZE - string_get_size(json->key));
    assert(json_entity->key_size == string_get_size(json->key));
    assert(json_entity->val_ptr == SECTION_SIZE - string_get_size(json->key) - sizeof(json->value));
    assert(json_entity->val_size == sizeof(json->value));
    assert(json_entity->fam_addr.dad_ptr == 500);
    assert(json_entity->fam_addr.bro_ptr == 0);
    assert(json_entity->fam_addr.son_ptr == 0);

    char *key_val = my_malloc_array(char, json_entity->key_size);
    RA_FREAD_OR_FAIL(key_val, sizeof(char) * json_entity->key_size, json_entity->key_ptr, file);
    assert(strncmp(key_val, json->key->val, json->key->cnt) == 0);
    free(key_val);

    int32_t val_val;
    RA_FREAD_OR_FAIL(&val_val, sizeof(int32_t), json_entity->val_ptr, file);
    assert(val_val == json->value.int32_val);

    json_dtor(json);

    entity_dtor(entity);
    entity_dtor(json_entity);

    free(header);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return write_status;
}

static status_t SectionExtents_WriteFloatJsonValue_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_FLOAT, json, "value", 5.5);
    ENTITY_INIT(entity, json, 500, 0, 0);

    fileoff_t save_json_addr;
    status_t write_status = sect_ext_write(extents, json, entity, &save_json_addr);
    assert(save_json_addr == sizeof(sect_head_entity_t));

    sect_head_entity_t *header = my_malloc(sect_head_entity_t);
    RA_FREAD_OR_FAIL(header, sizeof(sect_head_entity_t), 0, file);
    assert(header->free_space == (SECTION_SIZE - sizeof(sect_head_entity_t) - sizeof(entity_t) - sizeof(json->value) - string_get_size(json->key))); // 32 + 64 + 5 + 16
    assert(header->fst_rec_ptr == (SECTION_SIZE - sizeof(json->value) - string_get_size(json->key)));
    assert(header->lst_itm_ptr == sizeof(sect_head_entity_t) + sizeof(entity_t));

    entity_t *json_entity = entity_new();
    RA_FREAD_OR_FAIL(json_entity, sizeof(entity_t), save_json_addr, file);
    assert(json_entity->key_ptr == SECTION_SIZE - string_get_size(json->key));
    assert(json_entity->key_size == string_get_size(json->key));
    assert(json_entity->val_ptr == SECTION_SIZE - string_get_size(json->key) - sizeof(json->value));
    assert(json_entity->val_size == sizeof(json->value));
    assert(json_entity->fam_addr.dad_ptr == 500);
    assert(json_entity->fam_addr.bro_ptr == 0);
    assert(json_entity->fam_addr.son_ptr == 0);

    char *key_val = my_malloc_array(char, json_entity->key_size);
    RA_FREAD_OR_FAIL(key_val, sizeof(char) * json_entity->key_size, json_entity->key_ptr, file);
    assert(strncmp(key_val, json->key->val, json->key->cnt) == 0);
    free(key_val);

    float val_val;
    RA_FREAD_OR_FAIL(&val_val, sizeof(float), json_entity->val_ptr, file);
    assert(val_val == json->value.float_val);

    json_dtor(json);

    entity_dtor(entity);
    entity_dtor(json_entity);

    free(header);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return write_status;
}

static status_t SectionExtents_WriteBoolJsonValue_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    JSON_VALUE_INIT(TYPE_BOOL, json, "value", true);
    ENTITY_INIT(entity, json, 500, 0, 0);

    fileoff_t save_json_addr;
    status_t write_status = sect_ext_write(extents, json, entity, &save_json_addr);
    assert(save_json_addr == sizeof(sect_head_entity_t));

    sect_head_entity_t *header = my_malloc(sect_head_entity_t);
    RA_FREAD_OR_FAIL(header, sizeof(sect_head_entity_t), 0, file);
    assert(header->free_space == (SECTION_SIZE - sizeof(sect_head_entity_t) - sizeof(entity_t) - sizeof(json->value) - string_get_size(json->key)));
    assert(header->fst_rec_ptr == (SECTION_SIZE - sizeof(json->value) - string_get_size(json->key)));
    assert(header->lst_itm_ptr == sizeof(sect_head_entity_t) + sizeof(entity_t));

    entity_t *json_entity = entity_new();
    RA_FREAD_OR_FAIL(json_entity, sizeof(entity_t), save_json_addr, file);
    assert(json_entity->key_ptr == SECTION_SIZE - string_get_size(json->key));
    assert(json_entity->key_size == string_get_size(json->key));
    assert(json_entity->val_ptr == SECTION_SIZE - string_get_size(json->key) - sizeof(json->value));
    assert(json_entity->val_size == sizeof(json->value));
    assert(json_entity->fam_addr.dad_ptr == 500);
    assert(json_entity->fam_addr.bro_ptr == 0);
    assert(json_entity->fam_addr.son_ptr == 0);

    char *key_val = my_malloc_array(char, json_entity->key_size);
    RA_FREAD_OR_FAIL(key_val, sizeof(char) * json_entity->key_size, json_entity->key_ptr, file);
    assert(strncmp(key_val, json->key->val, json->key->cnt) == 0);
    free(key_val);

    bool val_val;
    RA_FREAD_OR_FAIL(&val_val, sizeof(bool), json_entity->val_ptr, file);
    assert(val_val == json->value.bool_val);

    json_dtor(json);

    entity_dtor(entity);
    entity_dtor(json_entity);

    free(header);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return write_status;
}

static status_t SectionExtents_WriteStringJsonValue_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    string_t *str = string_new();
    string_ctor(str, "hello world!", 12);

    JSON_VALUE_INIT(TYPE_STRING, json, "value", str);
    ENTITY_INIT(entity, json, 500, 0, 0);

    fileoff_t save_json_addr;
    status_t write_status = sect_ext_write(extents, json, entity, &save_json_addr);
    assert(save_json_addr == sizeof(sect_head_entity_t));

    sect_head_entity_t *header = my_malloc(sect_head_entity_t);
    RA_FREAD_OR_FAIL(header, sizeof(sect_head_entity_t), 0, file);
    assert(header->free_space == (SECTION_SIZE - sizeof(sect_head_entity_t) - sizeof(entity_t) - string_get_size(json->value.string_val) - string_get_size(json->key)));
    assert(header->fst_rec_ptr == (SECTION_SIZE - string_get_size(json->value.string_val) - string_get_size(json->key)));
    assert(header->lst_itm_ptr == sizeof(sect_head_entity_t) + sizeof(entity_t));

    entity_t *json_entity = entity_new();
    RA_FREAD_OR_FAIL(json_entity, sizeof(entity_t), save_json_addr, file);
    assert(json_entity->key_ptr == SECTION_SIZE - string_get_size(json->key));
    assert(json_entity->key_size == string_get_size(json->key));
    assert(json_entity->val_ptr == SECTION_SIZE - string_get_size(json->key) - string_get_size(json->value.string_val));
    assert(json_entity->val_size == string_get_size(json->value.string_val));
    assert(json_entity->fam_addr.dad_ptr == 500);
    assert(json_entity->fam_addr.bro_ptr == 0);
    assert(json_entity->fam_addr.son_ptr == 0);

    char *key_val = my_malloc_array(char, json_entity->key_size);
    RA_FREAD_OR_FAIL(key_val, sizeof(char) * json_entity->key_size, json_entity->key_ptr, file);
    assert(strncmp(key_val, json->key->val, json->key->cnt) == 0);
    free(key_val);

    char *val_val = my_malloc_array(char, json_entity->val_size);
    RA_FREAD_OR_FAIL(val_val, json_entity->val_size, json_entity->val_ptr, file);
    assert(strncmp(val_val, json->value.string_val->val, json->value.string_val->cnt) == 0);
    free(val_val);

    json_dtor(json);

    entity_dtor(entity);
    entity_dtor(json_entity);

    free(header);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return write_status;
}

static status_t SectionExtents_WriteStringJsonValueWithNotEnoughSpace_Failed()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    string_t *str = string_new();
    string_ctor(str, "hello world!", 12);

    JSON_VALUE_INIT(TYPE_STRING, json, "value", str);
    ENTITY_INIT(entity, json, 500, 0, 0);

    extents->header.free_space = 5;

    fileoff_t save_json_addr;
    status_t write_status = sect_ext_write(extents, json, entity, &save_json_addr);

    json_dtor(json);

    entity_dtor(entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return write_status;
}

static status_t SectionExtents_WriteObjectJsonValue_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_ext_t *extents = sect_ext_new();
    sect_ext_ctor(extents, 0, file);

    fileoff_t parent_json_addr = 1000;
    fileoff_t object_json_addr;

    JSON_VALUE_INIT(TYPE_OBJECT, object_json, "parent", NULL);
    ENTITY_INIT(entity, object_json, 1000, 0, 0);

    fileoff_t save_json_addr;
    status_t write_status = sect_ext_write(extents, object_json, entity, &object_json_addr);
    assert(object_json_addr == sizeof(sect_head_entity_t));

    entity_t *object_json_entity = entity_new();
    RA_FREAD_OR_FAIL(object_json_entity, sizeof(entity_t), object_json_addr, extents->header.filp);
    assert(object_json_entity->key_ptr == SECTION_SIZE - string_get_size(object_json->key));
    assert(object_json_entity->key_size == string_get_size(object_json->key));
    assert(object_json_entity->val_ptr == SECTION_SIZE - string_get_size(object_json->key));
    assert(object_json_entity->val_size == 0);
    assert(object_json_entity->fam_addr.dad_ptr == 1000);
    assert(object_json_entity->fam_addr.bro_ptr == 0);
    assert(object_json_entity->fam_addr.son_ptr == 0);
    assert(object_json_entity->type == TYPE_OBJECT);

    json_dtor(object_json);

    entity_dtor(entity);
    entity_dtor(object_json_entity);

    sect_ext_dtor(extents);

    fclose(file);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_extents_write()
{
    assert(SectionExtents_DefaultCtor_InvokeHeaderCtor() == OK);

    assert(SectionExtents_InvokeSync_InvokeHeaderSync() == OK);

    assert(SectionExtents_WriteInt32JsonValue_Successful() == OK);
    assert(SectionExtents_WriteFloatJsonValue_Successful() == OK);
    assert(SectionExtents_WriteBoolJsonValue_Successful() == OK);
    assert(SectionExtents_WriteStringJsonValue_Successful() == OK);
    assert(SectionExtents_WriteStringJsonValueWithNotEnoughSpace_Failed() == FAILED);

    assert(SectionExtents_WriteObjectJsonValue_Successful() == OK);
}
