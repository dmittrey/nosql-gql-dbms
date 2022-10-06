#pragma once

#include <stdio.h>

#include "xml.h"

typedef u_int64_t fileoff_t;

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

/*
Struct of Documents section:
            --------------------------
            | HEADER                 |
            --------------------------
content     | length      | next_doc |
            --------------------------
size(bytes) | 8           | 8        |
            -------------------------------------------------------
            | Documents                                           |
            -------------------------------------------------------
content     | name_length | name     | first_node_addr | next_sec |
            -------------------------------------------------------
size(bytes) |  8          | unknw    | 8               | 8        |
            -------------------------------------------------------

TOTAL SECTION SIZE IS 8KB => RAM size is constant
*/
struct Documents {
    fileoff_t location;
    fileoff_t size;
    fileoff_t next;
    struct Document* document[];
};

/*
Struct of Extent section:
            --------------------------------------
            | HEADER                             |
            --------------------------------------
content     | length | next_extent | prev_extent |
            --------------------------------------
size(bytes) | 8      | 8           | 8           |
            ------------------------------------------------------------------------------------------------
            | Nodes                                                                                        |
            ------------------------------------------------------------------------------------------------
content     | doc_addr     | parent_addr | name_len    | name       | type     | attr_name_len | attr_name | 
            ------------------------------------------------------------------------------------------------
size(bytes) | 8            | 8           | 8           | unknw      | 1        | 8             | unknw     | 
            ------------------------------------------------------------------------------------------------
content     | attr_val_len | attr_val    | child_count | child_addr | text_len | text          |
            ------------------------------------------------------------------------------------
size(bytes) | 8            | unknw       | 8           | 8          | 8        | unknw         |
            ------------------------------------------------------------------------------------

TOTAL SECTION SIZE IS 8KB => RAM size is constant
*/
struct Extent {
    fileoff_t location;
    fileoff_t size;
    fileoff_t next;
    fileoff_t prev;
    struct Node *nodes[];
};