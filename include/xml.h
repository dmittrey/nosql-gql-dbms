#pragma once 

#include "utils.h"

enum TypeOfNode {
    START_TAG = 0,
    END_TAG,
    TEXT,
    EMPTY_TAG
};

struct Attribute {
    string_t name;
    string_t value;
    struct Attribute *next; // Nullable
};

struct Node {
    struct Document *document;
    struct Node *parent;
    string_t name;
    enum TypeOfNode *type;
    struct Attribute* attribute; // Nullable
    struct Node* children; // Nullable
    string_t text; // Nullable(terminal tags)
};

struct Document {
    string_t name; 
    struct Node *first_node; // Nullable
};