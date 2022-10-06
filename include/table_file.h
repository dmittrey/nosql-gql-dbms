#pragma once

#include <stdio.h>

#include "xml.h"
#include "table.h"

typedef const char* file_path_t;

/* 
Abstraction to manipulate with data representation in file

Reponsibilities:
    - Handling sections
    - Work with data and perform queries
    - Open and close file  
*/

// Own program abstraction above file 
typedef FILE * table_file_t;

// Abstraction of performing ops
struct TransactionResult {
    table_file_t file; // Nullable or ptr to start queried data
    enum PerformStatus status;
};

/* Utility */

#define TRANSACTION_OK(file_ptr) (struct TransactionResult) { \
            .file = file_ptr, \
            .status = 0 \
        }
#define TRANSACTION_FAILED() (struct TransactionResult) { \
            .file = NULL, \
            .status = 1 \
        }

#define IS_VALID_TRANSACTION(transaction_result) transaction_result.status

/* File manipulation */

struct TransactionResult openOrCreateTableStoreFile(file_path_t file_path);

struct TransactionResult closeTableStoreFile(table_file_t *file);

/* Section manipulation */

enum PerformStatus createDocumentsSection(
    table_file_t *file,
    struct Documents* prev_section // Can be null if section is first
);

enum PerformStatus createExtentSection(
    table_file_t *file,
    struct Extent* prev_section // Can be null if section is first
);

/* Data manipulation */

struct TransactionResult insertDocument(table_file_t *file, struct Document *document);

struct TransactionResult readDocument(table_file_t *file, struct Document *document);

struct TransactionResult updateDocument(table_file_t *file, struct Document *prev_document, struct Document *new_document);

struct TransactionResult deleteDocument(table_file_t *file, struct Document *document);

struct TransactionResult insertNode(table_file_t *file, struct Node *node);

struct TransactionResult readNode(table_file_t *file, struct Node *node);

struct TransactionResult updateNode(table_file_t *file, struct Node *prev_node, struct Node *new_node);

struct TransactionResult deleteNode(table_file_t *file, struct Node *node);

