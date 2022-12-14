#pragma once

#include "utils.h"

struct page_t;

struct page_t *page_new();

void page_ctor(struct page_t *);

void page_dtor(struct page_t *);

PerformStatus page_insert(struct page_t *);
PerformStatus page_update(struct page_t *);
PerformStatus page_remove(struct page_t *);
PerformStatus page_read(struct page_t *);