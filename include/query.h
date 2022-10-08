#pragma once

#include <stdio.h>

#include "xml.h"

enum OperationType {
    INSERT,
    READ,
    UPDATE,
    DELETE 
};

enum OperationDataType {
    NODE,
    DOCUMENT
};

struct QueryRequest {
    enum OperationType op_type;
    enum OperationDataType data_type;
    union data
    {
        struct Node *node;
        struct Document *document;
    };
};

/* 
    Struct returns buffer with data to receive by portions(we can seek)
    
    FILE has pointer to data start
*/
struct QueryResponse {
    FILE* data_start; // Nullable
    enum PerformStatus status;
};