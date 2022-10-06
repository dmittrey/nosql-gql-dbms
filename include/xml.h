#pragma once 

#include "utils.h"

enum TypeOfNode {
    START_TAG = 0, // <address>
    END_TAG, // </address>
    TEXT,
    EMPTY_TAG // <hr />
};

struct Attribute {
    struct string_t name;
    struct string_t value;
};

struct Node {
    struct Document *document;
    struct Node *parent;
    struct string_t name;
    enum TypeOfNode *type;
    struct Attribute** attribute; // Nullable
    struct Node* children; // Nullable
    struct string_t text; // Nullable(terminal tags)
};

struct Document {
    struct string_t name; 
    struct Node *first_node; // Nullable
};