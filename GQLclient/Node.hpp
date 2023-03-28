#pragma once

#include <string>
#include <iostream>
#include <vector>

enum NodeType
{
    WORD = 0,

    DOUBLE_FIELD,
    INT32_FIELD,
    STRING_FIELD,
    BOOL_FIELD,

    DOUBLE_PROPERTY,
    INT32_PROPERTY,
    STRING_PROPERTY,
    BOOL_PROPERTY,

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

enum Command
{
    INSERT,
    DELETE,
    SELECT,
    UPDATE
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
    NodeType type_;

    Node() {}
    Node(NodeType type) : type_(type) {}

    virtual std::string repr() { return "Hello from Node!"; }
};

/* ---------------------- FIELD NODES ---------------------- */

struct FieldNode : Node
{
protected:
    std::string name_;

public:
    FieldNode() {}
    FieldNode(NodeType type, std::string &name) : Node(type), name_(name) {}

    std::string repr() override { return "|Field| name: " + name_; }
};

struct StringFieldNode : FieldNode
{
protected:
    std::string value_;

public:
    StringFieldNode(std::string &name, std::string &value) : FieldNode(STRING_FIELD, name), value_(value) {}

    std::string repr() override { return "|StringField| name: " + name_ + "\tvalue: " + value_; }
};

struct IntFieldNode : FieldNode
{
protected:
    std::string name_;
    int32_t value_;

public:
    IntFieldNode(std::string &name, int32_t &value) : FieldNode(INT32_FIELD, name), value_(value) {}

    std::string repr() override { return "|IntField| name: " + name_ + "\tvalue: " + std::to_string(value_); }
};

struct DoubleFieldNode : FieldNode
{
protected:
    std::string name_;
    double value_;

public:
    DoubleFieldNode(std::string &name, double &value) : FieldNode(DOUBLE_FIELD, name), name_(name), value_(value) {}

    std::string repr() override { return "|DoubleField| name: " + name_ + "\tvalue: " + std::to_string(value_); }
};

struct BoolFieldNode : FieldNode
{
protected:
    std::string name_;
    bool value_;

public:
    BoolFieldNode(std::string &name, bool &value) : FieldNode(BOOL_FIELD, name), value_(value) {}

    std::string repr() override { return "|BoolField| name: " + name_ + "\tvalue: " + std::to_string(value_); }
};

struct FieldListNode : Node
{
    std::vector<FieldNode> fields = {};

    void add(FieldNode &field) { fields.push_back(field); }
};

/* --------------------------------------------------------------- */

/* ---------------------- CONDITIONAL NODES ---------------------- */

struct PropertyNode : Node
{
protected:
    std::string name_;
    Cmp cmp_;

public:
    PropertyNode() {}
    PropertyNode(NodeType type, std::string &name, Cmp cmp) : Node(type), name_(name), cmp_(cmp) {}

    std::string repr() override { return "|Property| name: " + name_; }
};

struct StringPropertyNode : PropertyNode
{
protected:
    std::string val_;

public:
    StringPropertyNode(std::string &name, Cmp cmp, std::string &val) : PropertyNode(STRING_PROPERTY, name, cmp), val_(val) {}

    std::string repr() override { return "|StringProperty| name: " + name_ + "\tCmp: " + std::to_string(cmp_) + "\tValue: " + val_; }
};

struct IntPropertyNode : PropertyNode
{
protected:
    int32_t val_;

public:
    IntPropertyNode(std::string &name, Cmp cmp, int32_t &val) : PropertyNode(STRING_PROPERTY, name, cmp), val_(val) {}

    std::string repr() override { return "|IntProperty| name: " + name_ + "\tCmp: " + std::to_string(cmp_) + "\tValue: " + std::to_string(val_); }
};

struct DoublePropertyNode : PropertyNode
{
protected:
    double val_;

public:
    DoublePropertyNode(std::string &name, Cmp cmp, double &val) : PropertyNode(STRING_PROPERTY, name, cmp), val_(val) {}

    std::string repr() override { return "|DoubleProperty| name: " + name_ + "\tCmp: " + std::to_string(cmp_) + "\tValue: " + std::to_string(val_); }
};

struct BoolPropertyNode : PropertyNode
{
protected:
    bool val_;

public:
    BoolPropertyNode(std::string &name, Cmp cmp, bool &val) : PropertyNode(STRING_PROPERTY, name, cmp), val_(val) {}

    std::string repr() override { return "|BoolProperty| name: " + name_ + "\tCmp: " + std::to_string(cmp_) + "\tValue: " + std::to_string(val_); }
};

struct PropertyListNode : Node
{
    std::vector<PropertyNode> fields = {};

    void add(PropertyNode &field) { fields.push_back(field); }
};

/* --------------------------------------------------------------- */

/* ---------------------- WORD NODES ----------------------------- */

struct WordListNode : Node
{
    std::vector<std::string> fields = {};

    void add(std::string &field) { fields.push_back(field); }
};

/* --------------------------------------------------------------- */

/* ---------------------- ALIAS NODES ---------------------------- */

struct ReprNode : WordListNode
{
    ReprNode() {}
    ReprNode(const WordListNode &wordList) { fields = std::vector<std::string>{wordList.fields}; }
};

struct ConditionNode : PropertyListNode
{
    ConditionNode() {}
    ConditionNode(const PropertyListNode &propertyList) { fields = std::vector<PropertyNode>{propertyList.fields}; }
};

struct EntityNode : FieldListNode
{
    EntityNode() {}
    EntityNode(const FieldListNode &propertyList) { fields = std::vector<FieldNode>{propertyList.fields}; }
};

/* --------------------------------------------------------------- */

/* ---------------------- BODY NODES ----------------------------- */

struct ConditionBodyNode : Node
{
protected:
    std::string typeName_;
    ConditionNode conditionNode_;

public:
    ConditionBodyNode() {}
    ConditionBodyNode(const std::string &typeName, const ConditionNode &conditionNode) : typeName_(typeName), conditionNode_(conditionNode) {}

    std::string repr() override { return "GQLConditionBody"; }
};

struct EntityBodyNode : Node
{
protected:
    std::string typeName_;
    EntityNode entityNode_;

public:
    EntityBodyNode() {}
    EntityBodyNode(const std::string &typeName, const EntityNode &entityNode) : typeName_(typeName), entityNode_(entityNode) {}

    std::string repr() override
    {
        return std::string{"GQLEntityBody"} + 
                            "├─GQLWord " + 
                            "└─GQLEntity ";
    }
};

/* --------------------------------------------------------------- */

/* ---------------------- QUERY NODES ----------------------------- */

struct QueryNode : Node
{
protected:
    Command command_;
    ReprNode reprNode_;

public:
    QueryNode() {}
    QueryNode(Command command, const ReprNode &reprNode) : command_(command), reprNode_(reprNode) {}
};

struct InsertQueryNode : QueryNode
{
protected:
    EntityBodyNode entityBodyNode_;

public:
    InsertQueryNode(Command command, const EntityBodyNode &entityBodeNode, const ReprNode &reprNode) : QueryNode{command, reprNode}, entityBodyNode_(entityBodeNode) {}
};

struct SelectQueryNode : QueryNode
{
protected:
    ConditionBodyNode conditionBodyNode_;

public:
    SelectQueryNode(Command command, const ConditionBodyNode &conditionBodyNode, const ReprNode &reprNode) : QueryNode{command, reprNode}, conditionBodyNode_(conditionBodyNode) {}
};

struct DeleteQueryNode : QueryNode
{
protected:
    ConditionBodyNode conditionBodyNode_;

public:
    DeleteQueryNode(Command command, const ConditionBodyNode &conditionBodyNode, const ReprNode &reprNode) : QueryNode{command, reprNode}, conditionBodyNode_(conditionBodyNode) {}
};

struct UpdateQueryNode : QueryNode
{
protected:
    ConditionBodyNode conditionBodyNode_;
    EntityNode entityNode_;

public:
    UpdateQueryNode(Command command, const ConditionBodyNode &conditionBodyNode, const EntityNode &entityNode, const ReprNode &reprNode) : QueryNode{command, reprNode}, conditionBodyNode_(conditionBodyNode), entityNode_(entityNode) {}
};

/* --------------------------------------------------------------- */