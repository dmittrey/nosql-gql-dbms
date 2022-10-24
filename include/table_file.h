#pragma once

#include <stdio.h>

#include "file_utils.h"
#include "table.h"

/* 
Abstraction to manipulate with data representation in file

Reponsibilities:
    - Handling sections
    - Work with data and perform queries
    - Open and close file  
*/

/* File manipulation */

struct TransactionResult openOrCreateTableStoreFile(file_path_t file_path);

struct TransactionResult closeTableStoreFile(table_file_t *file);

/* Section manipulation */

enum PerformStatus createDocumentsSection(
    table_file_t *table_file,
    fileoff_t location,
    struct Documents* prev_section // Can be null if section is first
);

enum PerformStatus createExtentSection(
    table_file_t *table_file,
    fileoff_t location,
    struct Extent* prev_section // Can be null if section is first
);

/* Data manipulation */

enum PerformStatus insertDocumentNode(table_file_t *file, struct Document *document);

enum PerformStatus readDocumentNode(table_file_t *file, struct string_t name, struct Document* res_document);

enum PerformStatus updateDocumentNode(table_file_t *file, struct Document *prev_document, struct Document *new_document);

enum PerformStatus deleteDocumentNode(table_file_t *file, struct string_t *doc_name);

enum PerformStatus insertNode(table_file_t *file, struct Node *node);

enum PerformStatus readNode(table_file_t *file, fileoff_t location, struct Node *res_node);

enum PerformStatus updateNode(table_file_t *file, struct Node *prev_node, struct Node *new_node);

enum PerformStatus deleteNode(table_file_t *file, struct Node *node);

