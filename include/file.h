#pragma once

#include <stdio.h>

#include "xml.h"
#include "table.h"

/* 
Abstraction to manipulate with data representation in file

Reponsibilities:
    - Handling sections
    - Work with data and perform queries
    - Open and close file  
*/

// Own program abstraction above file 
struct TableFile {
    FILE *file;
};

struct TransactionsResult {
    struct TableFile *file; // Nullable or ptr to start queried data
    enum PerformStatus status;
};

/* File manipulation */

struct TransactionsResult createOrCreateTableStoreFile();

struct TransactionsResult closeTableStoreFile();

/* Section manipulation */

enum PerformStatus createDocumentsSection(
    struct TableFile *file,
    struct Documents* prev_section // Can be null if section is first
);

enum PerformStatus createExtentSection(
    struct TableFile *file,
    struct Extent* prev_section // Can be null if section is first
);

/* Data manipulation */

struct TransactionsResult insertDocument(struct TableFile *file, struct Document *document);

struct TransactionsResult readDocument(struct TableFile *file, struct Document *document);

struct TransactionsResult updateDocument(struct TableFile *file, struct Document *prev_document, struct Document *new_document);

struct TransactionsResult deleteDocument(struct TableFile *file, struct Document *document);

struct TransactionsResult insertData(struct TableFile *file, struct Node *node);

struct TransactionsResult readData(struct TableFile *file, struct Node *node);

struct TransactionsResult updateData(struct TableFile *file, struct Node *prev_node, struct Node *new_node);

struct TransactionsResult deleteData(struct TableFile *file, struct Node *node);

