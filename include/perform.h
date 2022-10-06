#pragma once

#include "query.h"
#include "file.h"

/* 
Abstraction to manipulate with data in RAM

Reponsibilities:
    - Handling queries and requests a file change
    - Work with requests
*/

/* Polymorhism invokation file operations */

#define TABLE_ACTION(data_type, op_type) TABLE_ACTION_##data_type_##op_type

#define TABLE_ACTION_NODE_INSERT insertDocument
#define TABLE_ACTION_NODE_READ   readDocument
#define TABLE_ACTION_NODE_UPDATE updateDocument
#define TABLE_ACTION_NODE_DELETE deleteDocument

#define TABLE_ACTION_DOCUMENT_INSERT insertNode
#define TABLE_ACTION_DOCUMENT_READ   readNode
#define TABLE_ACTION_DOCUMENT_UPDATE updateNode
#define TABLE_ACTION_DOCUMENT_DELETE deleteNode

/* Perform queries */

struct QueryResponse performAction(struct TableFile *file, struct QueryRequest query_request);