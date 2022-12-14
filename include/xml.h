#pragma once

#include "utils.h"

struct document_t;
struct node_t;

enum TypeOfNode
{
    START_TAG = 0, // <address>
    END_TAG,       // </address>
    TEXT,
    EMPTY_TAG // <hr />
};

typedef struct
{
    string_t name;
    string_t value;
} attribute_t;

typedef struct
{
    struct document_t *document;
    struct node_t *parent;
    string_t *name;
    enum TypeOfNode *type;
    attribute_t **attribute;  // Nullable
    struct node_t **children; // Nullable
    string_t *text;           // Nullable(terminal tags)
} node_t;

typedef struct
{
    string_t *name;
    node_t *root; // Nullable
} document_t;