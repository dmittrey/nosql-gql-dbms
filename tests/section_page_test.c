#include "utils.h"

#include "section/section_page.h"

// Создание с существующим файлом и нулевым отступом
int SectionPage_DefaultCtor_Successful() {
    FILE* file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");

    section_page_t *page = section_page_new();
    section_page_ctor(page, file);

    printf("%llu, %llu, %llu, %llu", page->free_space, page->next, page->last_item_ptr, page->first_record_ptr);

    fclose(file);

    return 0;
}

// Создание с существующим файлом и ненулевым отступом
int SectionPage_CtorWithFileStartNotFromZero_Successful() {
    FILE* file = fopen("/Users/dmitry/Desktop/low-level-programming/test.txt", "r+");
    fseek(file, 5, SEEK_SET);

    section_page_t *page = section_page_new();
    section_page_ctor(page, file);

    printf("%llu, %llu, %llu, %llu", page->free_space, page->next, page->last_item_ptr, page->first_record_ptr);

    fclose(file);

    return 0;
}

// Создание с несуществующим файлом
int SectionPage_CtorWithUndefinedFile_Exception() {
    
}

int main() {
    SectionPage_CtorWithFileStartNotFromZero_Successful();
}