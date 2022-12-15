#pragma once

#include <inttypes.h>

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
            Last item ptr points to first free bit after all items
            First record points to last free bit before all records

TOTAL SECTION SIZE IS 8KB => RAM size is constant
*/

/*
Struct of Extent section:
            ------------------------------------------------------------------
            | HEADER                                                         |
            ------------------------------------------------------------------
content     | free_space   | next         | last_item_ptr | first_record_ptr |
            ------------------------------------------------------------------
size(bytes) | 8            | 8            | 8             | 8                |
            ---------------------------------------------------------------------------------------------------------------
            | Nodes                                                                                                       |
            ---------------------------------------------------------------------------------------------------------------
content     | doc_addr     | parent_addr  | name_len      | name             | type       | attr_count    | attr_name_len |
            ---------------------------------------------------------------------------------------------------------------
size(bytes) | 8            | 8            | 8             | unknw            | 1          | 8             | 8             |
            ---------------------------------------------------------------------------------------------------------------
content     | attr_name    | attr_val_len | attr_val      | child_count      | child_addr | text_len      | text          |
            ---------------------------------------------------------------------------------------------------------------
size(bytes) | unknw        | 8            | unknw         | 8                | 8          | 8             | unknw         |
            ---------------------------------------------------------------------------------------------------------------

TOTAL SECTION SIZE IS 8KB => RAM size is constant
*/

#define SECTION_SIZE 8192

#define ITEM_SIZE 8

typedef uint64_t fileoff_t;
typedef uint64_t sectoff_t;