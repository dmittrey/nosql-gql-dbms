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
    fileoff_t length;
    fileoff_t next;
};

#define NULL_NEXT 0

#define DOCUMENTS_SECTION_SIZE 8192

/*
Struct of Documents section:
            --------------------------
            | HEADER                 |
            --------------------------
content     | length      | next     |
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
    struct Header* header;
    struct Document* document[];
};

#define EXTENT_SECTION_SIZE 8192

/*
Struct of Extent section:
            ------------------------------
            | HEADER                     |
            ------------------------------
content     | length       | next        |
            ------------------------------
size(bytes) | 8            | 8           |
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
    struct Header* header;
    struct Node *nodes[];
};