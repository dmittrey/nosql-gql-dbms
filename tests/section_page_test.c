#include <assert.h>

#include "utils.h"
#include "table.h"

#include "section/section_header.h"

// Создание с существующим файлом и нулевым отступом
void SectionPage_DefaultCtor_Successful()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");

    section_header_t *page = section_header_new();
    section_header_ctor(page, file);

    assert(page->free_space == (SECTION_SIZE - section_header_size(page)));
    assert(page->next == 0); // Next section is undefined
    assert(page->last_item_ptr == section_header_size(page));
    assert(page->first_record_ptr == SECTION_SIZE);

    fclose(file);
}

// Создание с существующим файлом и ненулевым отступом
void SectionPage_CtorWithFileStartNotFromZero_Successful()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");
    fseek(file, 5, SEEK_SET);

    section_header_t *page = section_header_new();
    ssection_header_ctor(page, file);

    assert(page->free_space == (SECTION_SIZE - section_header_size(page)));
    assert(page->next == 0); // Next section is undefined
    assert(page->last_item_ptr == ftell(file) + section_header_size(page));
    assert(page->first_record_ptr == ftell(file) + SECTION_SIZE);

    fclose(file);
}

bool SectionPage_ShiftLastItemPtr_Successful()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");
    sectoff_t shift = 5;

    section_header_t *page = section_header_new();
    section_header_ctor(page, 0, file);

    PerformStatus perform_result = section_header_shift_last_item_ptr(page, shift);

    assert(perform_result == OK);
    assert(page->last_item_ptr == section_header_size(page) + shift);

    sectoff_t file_last_item_ptr;
    FSEEK_OR_FAIL(file, sizeof(page->free_space) + sizeof(page->next));
    FREAD_OR_FAIL(&file_last_item_ptr, sizeof(sectoff_t), file);
    assert(file_last_item_ptr == section_header_size(page) + shift);

    return true;
}

bool SectionPage_ShiftFirstRecordPtr_Successful()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");
    sectoff_t shift = -5;

    section_header_t *page = section_header_new();
    section_header_ctor(page, 0, file);

    PerformStatus perform_result = section_header_shift_first_record_ptr(page, shift);

    assert(perform_result == OK);
    assert(page->first_record_ptr == SECTION_SIZE + shift);

    sectoff_t file_first_record_ptr;
    FSEEK_OR_FAIL(file, sizeof(page->free_space) + sizeof(page->next) + sizeof(page->last_item_ptr));
    FREAD_OR_FAIL(&file_first_record_ptr, sizeof(sectoff_t), file);
    assert(file_first_record_ptr == SECTION_SIZE + shift);

    return true;
}

bool SectionPage_SyncAfterUpdateInnerState_Successful()
{
    FILE *file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");
    section_header_t *page = section_header_new();

    page->free_space = 5;
    page->next = 6;
    page->last_item_ptr = 7;
    page->first_record_ptr = 8;
    page->filp = file;

    section_page_sync(page);
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
void SectionPage_CtorWithUndefinedFile_Exception()
{
    FILE *file = NULL;

    section_header_t *page = section_header_new();
    section_header_ctor(page, file);
}

int main()
{
    SectionPage_DefaultCtor_Successful();
    SectionPage_CtorWithFileStartNotFromZero_Successful();
    SectionPage_ShiftLastItemPtr_Successful();
    SectionPage_ShiftFirstRecordPtr_Successful();
    SectionPage_SyncAfterUpdateInnerState_Successful();
}