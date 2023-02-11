#include <assert.h>

#include "section/section_extents.h"

void SectionExtents_DefaultCtor_InvokeHeaderCtor()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    assert(extents->header.free_space == (SECTION_SIZE - section_header_size(extents->header)));
    assert(extents->header.next == 0); // Next section is undefined
    assert(extents->header.last_item_ptr == section_header_size(extents->header));
    assert(extents->header.first_record_ptr == SECTION_SIZE);

    fclose(file);
}

bool SectionExtents_InvokeSync_InvokeHeaderSync()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");
    section_extents_t *extents = section_extents_new();

    extents->header.free_space = 5;
    extents->header.next = 6;
    extents->header.last_item_ptr = 7;
    extents->header.first_record_ptr = 8;
    extents->header.filp = file;

    section_extents_sync(extents);
    // TODO Refactor to macro
    sectoff_t file_free_space;
    FREAD_OR_FAIL(&file_free_space, sizeof(sectoff_t), file);
    assert(file_free_space == 5);

    sectoff_t file_next;
    FREAD_OR_FAIL(&file_next, sizeof(sectoff_t), file);
    assert(file_next == 6);

    sectoff_t file_last_item_ptr;
    FREAD_OR_FAIL(&file_last_item_ptr, sizeof(sectoff_t), file);
    assert(file_last_item_ptr == 7);

    sectoff_t file_first_record_ptr;
    FREAD_OR_FAIL(&file_first_record_ptr, sizeof(sectoff_t), file);
    assert(file_first_record_ptr == 8);

    return true;
}

void SectionExtents_WriteInt32JsonValue_Successful()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");

    section_extents_t *extents = section_extents_new();
    section_extents_ctor(extents, 0, file);

    json_value_t *json = json_value_new();
    
    json->type = TYPE_INT32;
    json->value.int32_val = 5;
    json->object.attributes_count = 0;

    fileoff_t save_json_addr = 0;

    section_extents_write(extents, json, 0, &save_json_addr);

    printf("%lld\n", save_json_addr);
    printf("%lld\n", section_header_size(extents->header));
    assert(save_json_addr == section_header_size(extents->header));
}

void SectionExtents_WriteFloatJsonValue_Successful()
{
}

void SectionExtents_WriteStringJsonValue_Successful()
{
}

void SectionExtents_WriteBoolJsonValue_Successful()
{
}

void SectionExtents_WriteObjectJsonValue_Successful()
{
}

void SectionExtents_WriteStringJsonValueWithNotEnoughSpace_Failed()
{
}

int main()
{
    // SectionExtents_DefaultCtor_InvokeHeaderCtor();
    // assert(SectionExtents_InvokeSync_InvokeHeaderSync() == true);
    SectionExtents_WriteInt32JsonValue_Successful();
    SectionExtents_WriteFloatJsonValue_Successful();
    SectionExtents_WriteStringJsonValue_Successful();
    SectionExtents_WriteBoolJsonValue_Successful();
    SectionExtents_WriteObjectJsonValue_Successful();
    SectionExtents_WriteStringJsonValueWithNotEnoughSpace_Failed();
}
