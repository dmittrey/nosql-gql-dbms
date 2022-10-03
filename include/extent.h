#pragma once

#include <stdio.h>

#include "table_types.h"

typedef u_int64_t pr_key_t;

typedef struct Extent {
    struct Header *header;
    struct Item *item;
    struct Record *record;
    size_t free_space;
} Extent;

typedef struct Header {
    size_t next_extent;
    size_t prev_extent;
    struct Item *first_item;
    struct Item *last_item;
} Header;

typedef struct Item {
    struct Item *next;
    struct Record *record;
} Item;

typedef struct Record {
    pr_key_t key;
    size_t lenght;
    TableStoreRecord *params[];
} Record;