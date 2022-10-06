#pragma once

#include <stdio.h>

#include "xml.h"
#include "utils.h"

enum OperationType {
    INSERT,
    READ,
    UPDATE,
    DELETE 
};

enum PerformStatus {
    OK = 0,
    FAILED = 1
};

struct QueryRequest {
    OperationType op_type;
    Node *node;
};

/* 
    Struct returns buffer with data to receive by portions(we can seek)
    
    FILE has pointer to data start
*/
struct QueryResponse {
    FILE* data_start; // Nullable
    enum PerformStatus status;
};