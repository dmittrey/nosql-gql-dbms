#pragma once

#include "utils.h"

enum TypeOfNode
{
    START_TAG = 0, // <address>
    END_TAG,       // </address>
    TEXT,
    EMPTY_TAG // <hr />
};

struct Attribute
{
    string_t name;
    string_t value;
};

struct Node
{
    struct Document *document;
    struct Node *parent;
    string_t *name;
    enum TypeOfNode *type;
    struct Attribute **attribute; // Nullable
    struct Node **children;       // Nullable
    string_t *text;               // Nullable(terminal tags)
};

struct document_t;

typedef struct
{
    string_t *name;
    struct Node *first_node; // Nullable
    struct document_t *next;
} document_t;