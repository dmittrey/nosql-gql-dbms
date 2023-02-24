#include <assert.h>

#include "utils.h"
#include "table.h"

#include "memory/section/header.h"
#include "physical/section/header.h"

static const char *test_file_name = "test.bin";

// Создание с существующим файлом и нулевым отступом
status_t SectionHeader_DefaultCtor_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    sect_head_t *header = sect_head_new();
    status_t ctor_status = sect_head_ctor(header, 0, file);

    assert(header->free_space == (SECTION_SIZE - sizeof(sect_head_entity_t)));
    assert(header->next_ptr == 0); // Next section is undefined
    assert(header->lst_itm_ptr == sizeof(sect_head_entity_t));
    assert(header->fst_rec_ptr == SECTION_SIZE);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return ctor_status;
}

// Создание с существующим файлом и ненулевым отступом
status_t SectionHeader_CtorWithFileStartNotFromZero_Successful()
{
    FILE *file = fopen(test_file_name, "w+");
    sectoff_t shift = 5;

    sect_head_t *header = sect_head_new();
    status_t ctor_status = sect_head_ctor(header, shift, file);

    assert(header->free_space == (SECTION_SIZE - sizeof(sect_head_entity_t)));
    assert(header->next_ptr == 0); // Next section is undefined
    assert(header->lst_itm_ptr == shift + sizeof(sect_head_entity_t));
    assert(header->fst_rec_ptr == shift + SECTION_SIZE);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return ctor_status;
}

status_t SectionHeader_ShiftLastItemPtr_Successful()
{
    FILE *file = fopen(test_file_name, "w+");
    sectoff_t shift = 5;

    sect_head_t *header = sect_head_new();
    sect_head_ctor(header, 0, file);

    status_t perform_result = sect_head_shift_lst_itm_ptr(header, shift);

    assert(perform_result == OK);
    assert(header->lst_itm_ptr == sizeof(sect_head_entity_t) + shift);

    sectoff_t file_last_item_ptr;
    FSEEK_OR_FAIL(file, sizeof(header->free_space) + sizeof(header->next_ptr));
    FREAD_OR_FAIL(&file_last_item_ptr, sizeof(sectoff_t), file);
    assert(file_last_item_ptr == sizeof(sect_head_entity_t) + shift);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t SectionHeader_ShiftFirstRecordPtr_Successful()
{
    FILE *file = fopen(test_file_name, "w+");
    sectoff_t shift = -5;

    sect_head_t *header = sect_head_new();
    sect_head_ctor(header, 0, file);

    status_t perform_result = sect_head_shift_fst_rec_ptr(header, shift);

    assert(perform_result == OK);
    assert(header->fst_rec_ptr == SECTION_SIZE + shift);

    sectoff_t file_first_record_ptr;
    FSEEK_OR_FAIL(file, sizeof(header->free_space) + sizeof(header->next_ptr) + sizeof(header->lst_itm_ptr));
    FREAD_OR_FAIL(&file_first_record_ptr, sizeof(sectoff_t), file);
    assert(file_first_record_ptr == SECTION_SIZE + shift);

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

status_t SectionHeader_SyncAfterUpdateInnerState_Successful()
{
    FILE *file = fopen(test_file_name, "w+");
    sect_head_t *header = sect_head_new();

    header->free_space = 5;
    header->next_ptr = 6;
    header->lst_itm_ptr = 7;
    header->fst_rec_ptr = 8;
    header->filp = file;

    sect_head_sync(header);

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

    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

// Создание с несуществующим файлом
status_t SectionHeader_CtorWithUndefinedFile_Exception()
{
    FILE *file = NULL;

    sect_head_t *header = sect_head_new();
    return sect_head_ctor(header, 0, file);
}

void test_header()
{
    assert(SectionHeader_DefaultCtor_Successful() == OK);
    assert(SectionHeader_CtorWithFileStartNotFromZero_Successful() == OK);
    assert(SectionHeader_ShiftLastItemPtr_Successful() == OK);
    assert(SectionHeader_ShiftFirstRecordPtr_Successful() == OK);
    assert(SectionHeader_SyncAfterUpdateInnerState_Successful() == OK);
}