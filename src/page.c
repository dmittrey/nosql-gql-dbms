#include "page_p.h"
#include "utils.h"

static void __page_insert(void *this_ptr)
{
    page_t *page = (page_t *)this_ptr;
}

static void __page_update(void *this_ptr)
{
    page_t *page = (page_t *)this_ptr;
}

static void __page_remove(void *this_ptr)
{
    page_t *page = (page_t *)this_ptr;
}

static void __page_read(void *this_ptr)
{
    page_t *page = (page_t *)this_ptr;
}

void page_ctor(page_t *page)
{
    page->insert_func = __page_insert;
    page->update_func = __page_update;
    page->remove_func = __page_remove;
    page->read_func = __page_read;
}

enum PerformStatus page_insert(page_t *page)
{
    page->insert_func(page);
    return FAILED;
}

enum PerformStatus page_update(page_t *page)
{
    page->update_func(page);
    return FAILED;
}

enum PerformStatus page_remove(page_t *page)
{
    page->remove_func(page);
    return FAILED;
}

enum PerformStatus page_read(page_t *page)
{
    page->read_func(page);
    return FAILED;
}