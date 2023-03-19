#include "tests/test.h"

#include "section/header.h"
#include "section/header_p.h"

static const char *test_file_name = "test.bin";

// Создание с существующим файлом и нулевым отступом
static Status SectionHeader_DefaultCtor_Successful()
{
    FILE *file = fopen(test_file_name, "w+");

    Sect_head *header = sect_head_new();
    Status ctor_Status = sect_head_ctor(header, 0, file);

    assert(header->free_space == (SECTION_SIZE - sizeof(Sect_head_entity)));
    assert(header->next_ptr == 0); // Next section is undefined
    assert(header->lst_itm_ptr == sizeof(Sect_head_entity));
    assert(header->fst_rec_ptr == SECTION_SIZE);

    sect_head_dtor(header);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return ctor_Status;
}

// Создание с существующим файлом и ненулевым отступом
static Status SectionHeader_CtorWithFileStartNotFromZero_Successful()
{
    FILE *file = fopen(test_file_name, "w+");
    Sectoff shift = 5;

    Sect_head *header = sect_head_new();
    Status ctor_Status = sect_head_ctor(header, shift, file);

    assert(header->free_space == (SECTION_SIZE - sizeof(Sect_head_entity)));
    assert(header->next_ptr == 0); // Next section is undefined
    assert(header->lst_itm_ptr == sizeof(Sect_head_entity));
    assert(header->fst_rec_ptr == SECTION_SIZE);

    sect_head_dtor(header);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return ctor_Status;
}

static Status SectionHeader_ShiftLastItemPtr_Successful()
{
    FILE *file = fopen(test_file_name, "w+");
    Sectoff shift = 5;

    Sect_head *header = sect_head_new();
    sect_head_ctor(header, 0, file);

    Status perform_result = sect_head_shift_lip(header, shift);

    assert(perform_result == OK);
    assert(header->lst_itm_ptr == sizeof(Sect_head_entity) + shift);

    Sectoff file_last_item_ptr;
    FSEEK_OR_FAIL(file, sizeof(header->free_space) + sizeof(header->next_ptr));
    FREAD_OR_FAIL(&file_last_item_ptr, sizeof(Sectoff), file);
    assert(file_last_item_ptr == sizeof(Sect_head_entity) + shift);

    sect_head_dtor(header);

    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static Status SectionHeader_ShiftFirstRecordPtr_Successful()
{
    FILE *file = fopen(test_file_name, "w+");
    Sectoff shift = -5;

    Sect_head *header = sect_head_new();
    sect_head_ctor(header, 0, file);

    Status perform_result = sect_head_shift_frp(header, shift);

    assert(perform_result == OK);
    assert(header->fst_rec_ptr == SECTION_SIZE + shift);

    Sectoff file_first_record_ptr;
    FSEEK_OR_FAIL(file, sizeof(header->free_space) + sizeof(header->next_ptr) + sizeof(header->lst_itm_ptr));
    FREAD_OR_FAIL(&file_first_record_ptr, sizeof(Sectoff), file);
    assert(file_first_record_ptr == SECTION_SIZE + shift);

    sect_head_dtor(header);
    
    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

static Status SectionHeader_SyncAfterUpdateInnerState_Successful()
{
    FILE *file = fopen(test_file_name, "w+");
    Sect_head *header = sect_head_new();

    header->free_space = 5;
    header->next_ptr = 6;
    header->lst_itm_ptr = 7;
    header->fst_rec_ptr = 8;
    header->filp = file;

    sect_head_sync(header);

    Sectoff file_free_space;
    FREAD_OR_FAIL(&file_free_space, sizeof(Sectoff), file);
    assert(file_free_space == 5);

    Sectoff file_next;
    FREAD_OR_FAIL(&file_next, sizeof(Sectoff), file);
    assert(file_next == 6);

    Sectoff file_last_item_ptr;
    FREAD_OR_FAIL(&file_last_item_ptr, sizeof(Sectoff), file);
    assert(file_last_item_ptr == 7);

    Sectoff file_first_record_ptr;
    FREAD_OR_FAIL(&file_first_record_ptr, sizeof(Sectoff), file);
    assert(file_first_record_ptr == 8);

    sect_head_dtor(header);
    
    fclose(file);
    DO_OR_FAIL(remove(test_file_name));

    return OK;
}

void test_header()
{
    assert(SectionHeader_DefaultCtor_Successful() == OK);
    assert(SectionHeader_CtorWithFileStartNotFromZero_Successful() == OK);
    assert(SectionHeader_ShiftLastItemPtr_Successful() == OK);
    assert(SectionHeader_ShiftFirstRecordPtr_Successful() == OK);
    assert(SectionHeader_SyncAfterUpdateInnerState_Successful() == OK);
}