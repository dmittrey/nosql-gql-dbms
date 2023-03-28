#pragma once

#include <string>
#include <iostream>
#include <vector>

enum NodeType
{
    WORD = 0,
    COLON,
    FIELD,
    FIELD_LIST,
    PROPERTY,
    PROPERTY_LIST,
    WORD_LIST,
    REPR,
    CONDITIONAL,
    ENTITY,
    CONDITIONAL_BODY,
    ENTITY_BODY,
    QUERY,
    QUERY_LIST
};

enum Cmp
{
    GT = 0,
    GE,
    LT,
    LE,
    IN,
    EQ
};

struct Node
{
protected:
    Node *rhb;
    Node *lhb;
    NodeType type;

public:
    virtual void add_son(Node &node) {}
    virtual void print() {}
};

enum FieldType
{
    INT32,
    DOUBLE,
    BOOL,
    STRING
};

struct FieldNode : public Node
{
private:
    std::string field_;
    FieldType type_;
    int32_t int32_val_;
    double double_val_;
    bool bool_val_;
    std::string string_val_;

public:
    FieldNode() {}
    FieldNode(std::string &field, int32_t &val) : field_(field), int32_val_(val) { type_ = INT32; }
    FieldNode(std::string &field, double &val) : field_(field), double_val_(val) { type_ = DOUBLE; }
    FieldNode(std::string &field, bool &val) : field_(field), bool_val_(val) { type_ = BOOL; }
    FieldNode(std::string &field, std::string &val) : field_(field), string_val_(val) { type_ = STRING; }
    void print() override
    {
        std::cout << "Field: " << field_;
        switch (type_)
        {
        case INT32:
            std::cout << "\tVal: " << int32_val_ << std::endl;
            break;
        case DOUBLE:
            std::cout << "\tVal: " << double_val_ << std::endl;
            break;
        case BOOL:
            std::cout << "\tVal: " << bool_val_ << std::endl;
            break;
        case STRING:
            std::cout << "\tVal: " << string_val_ << std::endl;
            break;
        }
    }
};

struct PropertyNode : public Node
{
private:
    std::string field_;
    FieldType type_;
    Cmp cmp_;

    int32_t int32_val_;
    double double_val_;
    bool bool_val_;
    std::string string_val_;

public:
    PropertyNode() {}
    PropertyNode(std::string &field, FieldType type, Cmp cmp) : field_(field), type_(type), cmp_(cmp) {}
};

template <typename T>
struct ListNode : public Node
{
    std::vector<T> collection;

    void add_son(Node &node) override { collection.push_back(static_cast<T &>(node)); }
    void print() override
    {
        for (auto i : collection)
            i.print();
    }
};

struct WordListNode : public Node 
{
    std::vector<std::string> collection;
};

struct EntityBodyNode : public Node
{
    std::vector<FieldNode> collection_;
    std::string nodeSchemaName_;

    EntityBodyNode() {}
    EntityBodyNode(ListNode<FieldNode> &list, std::string &schemaName) : collection_(list.collection), nodeSchemaName_(schemaName) {}
};

struct ConditionalBodyNode : public Node
{
    std::vector<PropertyNode> collection_;
    std::string nodeSchemaName_;

    ConditionalBodyNode() {}
    ConditionalBodyNode(ListNode<PropertyNode> &list, std::string &schemaName) : collection_(list.collection), nodeSchemaName_(schemaName) {}
};

struct QueryNode : public Node
{
    
}