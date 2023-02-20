#include <assert.h>

#include "utils.h"
#include "table.h"

#include "section/section_header.h"

// Создание с существующим файлом и нулевым отступом
void SectionHeader_DefaultCtor_Successful()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");

    section_header_t *header = section_header_new();
    section_header_ctor(header, 0, file);

    assert(header->free_space == (SECTION_SIZE - section_header_size()));
    assert(header->next == 0); // Next section is undefined
    assert(header->last_item_ptr == section_header_size());
    assert(header->first_record_ptr == SECTION_SIZE);

    fclose(file);
}

// Создание с существующим файлом и ненулевым отступом
void SectionHeader_CtorWithFileStartNotFromZero_Successful()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");
    sectoff_t shift = 5;

    section_header_t *header = section_header_new();
    section_header_ctor(header, shift, file);

    assert(header->free_space == (SECTION_SIZE - section_header_size()));
    assert(header->next == 0); // Next section is undefined
    assert(header->last_item_ptr == shift + section_header_size());
    assert(header->first_record_ptr == shift + SECTION_SIZE);

    fclose(file);
}

bool SectionHeader_ShiftLastItemPtr_Successful()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");
    sectoff_t shift = 5;

    section_header_t *header = section_header_new();
    section_header_ctor(header, 0, file);

    PerformStatus perform_result = section_header_shift_last_item_ptr(header, shift);

    assert(perform_result == OK);
    assert(header->last_item_ptr == section_header_size() + shift);

    sectoff_t file_last_item_ptr;
    FSEEK_OR_FAIL(file, sizeof(header->free_space) + sizeof(header->next));
    FREAD_OR_FAIL(&file_last_item_ptr, sizeof(sectoff_t), file);
    assert(file_last_item_ptr == section_header_size() + shift);

    return true;
}

bool SectionHeader_ShiftFirstRecordPtr_Successful()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");
    sectoff_t shift = -5;

    section_header_t *header = section_header_new();
    section_header_ctor(header, 0, file);

    PerformStatus perform_result = section_header_shift_first_record_ptr(header, shift);

    assert(perform_result == OK);
    assert(header->first_record_ptr == SECTION_SIZE + shift);

    sectoff_t file_first_record_ptr;
    FSEEK_OR_FAIL(file, sizeof(header->free_space) + sizeof(header->next) + sizeof(header->last_item_ptr));
    FREAD_OR_FAIL(&file_first_record_ptr, sizeof(sectoff_t), file);
    assert(file_first_record_ptr == SECTION_SIZE + shift);

    return true;
}

bool SectionHeader_SyncAfterUpdateInnerState_Successful()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");
    section_header_t *header = section_header_new();

    header->free_space = 5;
    header->next = 6;
    header->last_item_ptr = 7;
    header->first_record_ptr = 8;
    header->filp = file;

    section_header_sync(header);
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

// Создание с несуществующим файлом
void SectionHeader_CtorWithUndefinedFile_Exception()
{
    FILE *file = NULL;

    section_header_t *header = section_header_new();
    section_header_ctor(header, 0, file);
}

// int main()
// {
//     SectionPage_DefaultCtor_Successful();
//     SectionPage_CtorWithFileStartNotFromZero_Successful();
//     SectionPage_ShiftLastItemPtr_Successful();
//     SectionPage_ShiftFirstRecordPtr_Successful();
//     SectionPage_SyncAfterUpdateInnerState_Successful();
// }