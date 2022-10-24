#pragma once

#include <stdio.h>
#include <inttypes.h>

#include "xml.h"

typedef uint64_t fileoff_t;

/*
Table representation in file:
-------------
| Documents |
-------------
| Extent    |
-------------
| Extent    |
-------------
| Documents |
-------------
| Extent    |
-------------

In other words, there is 2 random placed sections with information about docs and nodes.
*/
enum SectionType {
    DOCUMENTS,
    EXTENT
};

struct Header {
    fileoff_t free_space;
    fileoff_t next;
    fileoff_t last_item_ptr; // Pointer to first free cell(after items)
    fileoff_t first_record_ptr; // Pointer to last free cell(before records)
};

#define NULL_NEXT 0

#define DOCUMENTS_SECTION_SIZE 8192

#define ITEM_SIZE 8

/*
Struct of Documents section:
            ---------------------------------------------------------------
            | HEADER                                                      |
            ---------------------------------------------------------------
content     | free_space  | next     | last_item_ptr   | first_record_ptr |
            ---------------------------------------------------------------
size(bytes) | 8           | 8        | 8               | 8                |
            ---------------------------------------------------------------
            | Documents                                                   |
            ---------------------------------------------------------------
content     | name_length | name_ptr | first_node_addr |
            --------------------------------------------
size(bytes) |  8          | 8        | 8               |
            --------------------------------------------

TODO refactor image representation, firstly i want to rows of items(8b), 
then random size records 

Records increase from start to end. Names increase from end to start. 
It's useful in cases, when we would change name to longer or shorter

Contract :  All items point to records in same section !!!
            Last item ptr points to first bit after all items
            Firsty record ptr points to first filled by records bit

TOTAL SECTION SIZE IS 8KB => RAM size is constant
*/
struct Documents {
    struct Header* header;
    struct Document* document[];
};

#define EXTENT_SECTION_SIZE 8192

/*
Struct of Extent section:
            -----------------------------------------------------------------
            | HEADER                                                        |
            -----------------------------------------------------------------
content     | length       | next        | last_item_ptr | first_record_ptr |
            -----------------------------------------------------------------
size(bytes) | 8            | 8           | 8             | 8                |
            --------------------------------------------------------------------------------------------------------
            | Nodes                                                                                                |
            --------------------------------------------------------------------------------------------------------
content     | doc_addr     | parent_addr | name_len      | name             | type     | attr_name_len | attr_name | 
            --------------------------------------------------------------------------------------------------------
size(bytes) | 8            | 8           | 8             | unknw            | 1        | 8             | unknw     | 
            --------------------------------------------------------------------------------------------------------
content     | attr_val_len | attr_val    | child_count   | child_addr       | text_len | text          |
            --------------------------------------------------------------------------------------------
size(bytes) | 8            | unknw       | 8             | 8                | 8        | unknw         |
            --------------------------------------------------------------------------------------------

TOTAL SECTION SIZE IS 8KB => RAM size is constant
*/
struct Extent {
    struct Header* header;
    struct Node *nodes[];
};