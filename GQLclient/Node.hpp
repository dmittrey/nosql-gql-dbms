#pragma once

#include <string>
#include <iostream>
#include <vector>

enum FieldType
{
    STRING,
    INT32,
    DOUBLE,
    BOOL
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
    virtual std::string repr(int level) { return "Node"; };

protected:
    std::string offset(int level) { return std::string(level, '\t'); }
};

/* ---------------------- FIELD NODES ---------------------- */

struct FieldNode : Node
{
protected:
    FieldType type;
    std::string name_;
    std::string str_value_;
    int32_t int_value_;
    double double_value_;
    bool bool_value_;

public:
    FieldNode() {}
    FieldNode(std::string &name, std::string &value) : name_(name), str_value_(value), type(STRING) {}
    FieldNode(std::string &name, int32_t &value) : name_(name), int_value_(value), type(INT32) {}
    FieldNode(std::string &name, double &value) : name_(name), double_value_(value), type(DOUBLE) {}
    FieldNode(std::string &name, bool &value) : name_(name), bool_value_(value), type(BOOL) {}

    std::string repr(int level) override
    {
        std::string str = "GQLField name: " + name_ + "\tvalue: ";
        switch (type)
        {
        case STRING:
            str += str_value_;
            break;
        case INT32:
            str += std::to_string(int_value_);
            break;
        case DOUBLE:
            str += std::to_string(double_value_);
            break;
        case BOOL:
            str += std::to_string(bool_value_);
            break;
        }

        return str;
    }
};

struct FieldListNode : public Node
{
    std::vector<FieldNode> fields = {};

    void add(FieldNode &field) { fields.push_back(field); }

    std::string repr(int level) override
    {
        std::string result = std::string{"GQLFieldList"};
        for (auto i : fields)
        {
            result += "\n" + offset(level) + "└─" + i.repr(level);
        }
        return result;
    }
};

/* --------------------------------------------------------------- */

/* ---------------------- CONDITIONAL NODES ---------------------- */

struct PropertyNode : FieldNode
{
protected:
    Cmp cmp_;

public:
    PropertyNode() {}
    PropertyNode(std::string &name, Cmp cmp, std::string &value) : FieldNode{name, value}, cmp_(cmp) {}
    PropertyNode(std::string &name, Cmp cmp, int32_t &value) : FieldNode{name, value}, cmp_(cmp) {}
    PropertyNode(std::string &name, Cmp cmp, double &value) : FieldNode{name, value}, cmp_(cmp) {}
    PropertyNode(std::string &name, Cmp cmp, bool &value) : FieldNode{name, value}, cmp_(cmp) {}

    std::string repr(int level) override
    {
        std::string str = "GQLProperty name: " + name_ + "\tOperation: " + std::to_string(cmp_) + "\tvalue: ";
        switch (type)
        {
        case STRING:
            str += str_value_;
            break;
        case INT32:
            str += std::to_string(int_value_);
            break;
        case DOUBLE:
            str += std::to_string(double_value_);
            break;
        case BOOL:
            str += std::to_string(bool_value_);
            break;
        }

        return str;
    }
};

struct PropertyListNode : Node
{
    std::vector<PropertyNode> fields = {};

    void add(PropertyNode &field) { fields.push_back(field); }

    std::string repr(int level) override
    {
        std::string result = std::string{"GQLPropertyList"};
        for (auto i : fields)
        {
            result += "\n" + offset(level) + "└─" + i.repr(level);
        }
        return result;
    }
};

/* --------------------------------------------------------------- */

/* ---------------------- WORD NODES ----------------------------- */

struct WordListNode : Node
{
    std::vector<std::string> fields = {};

    void add(std::string field)
    {
        fields.push_back(field);
    }

    std::string repr(int level) override
    {
        std::string result = std::string{"GQLWordList"};
        for (auto i : fields)
        {
            result += "\n" + offset(level) + "└─GQLWord " + i;
        }
        return result;
    }
};

/* --------------------------------------------------------------- */

/* ---------------------- ALIAS NODES ---------------------------- */

struct ReprNode : Node
{
protected:
    WordListNode wordListNode_;

public:
    ReprNode() {}
    ReprNode(const WordListNode &wordListNode) : wordListNode_(wordListNode) {}

    std::string repr(int level) override
    {
        return std::string{"GQLRepresentation"} +
               "\n" + offset(level) + "└─" + wordListNode_.repr(level + 1);
    }
};

struct ConditionNode : Node
{
protected:
    PropertyListNode propertyListNode_;

public:
    ConditionNode() {}
    ConditionNode(const PropertyListNode &propertyListNode) : propertyListNode_(propertyListNode) {}

    std::string repr(int level) override
    {
        return std::string{"GQLCondition"} +
               "\n" + offset(level) + "└─" + propertyListNode_.repr(level + 1);
    }
};

struct EntityNode : Node
{
protected:
    FieldListNode fieldListNode_;

public:
    EntityNode() {}
    EntityNode(const FieldListNode &fieldListNode) : fieldListNode_(fieldListNode) {}

    std::string repr(int level) override
    {
        return std::string{"GQLEntity"} +
               "\n" + offset(level) + "└─" + fieldListNode_.repr(level + 1);
    }
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

    std::string repr(int level) override
    {
        return std::string{"GQLConditionBody"} +
               "\n" + offset(level) + "├─GQLWord " + typeName_ +
               "\n" + offset(level) + "└─" + conditionNode_.repr(level + 1);
    }
};

struct EntityBodyNode : Node
{
protected:
    std::string typeName_;
    EntityNode entityNode_;

public:
    EntityBodyNode() {}
    EntityBodyNode(const std::string &typeName, const EntityNode &entityNode) : typeName_(typeName), entityNode_(entityNode) {}

    std::string repr(int level) override
    {
        return std::string{"GQLEntityBody"} +
               "\n" + offset(level) + "├─GQLWord " + typeName_ +
               "\n" + offset(level) + "└─" + entityNode_.repr(level + 1);
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

    std::string repr(int level) override
    {
        return "Hello from query node!";
    }
};

struct InsertQueryNode : QueryNode
{
protected:
    EntityBodyNode entityBodyNode_;

public:
    InsertQueryNode() {}
    InsertQueryNode(Command command, const EntityBodyNode &entityBodeNode, const ReprNode &reprNode) : QueryNode{command, reprNode}, entityBodyNode_(entityBodeNode) {}

    std::string repr(int level) override
    {
        return std::string{"GQLInsertQuery"} +
               "\n├─GQLCommand " + std::to_string(command_) +
               "\n├─" + entityBodyNode_.repr(level + 1) +
               "\n└─" + reprNode_.repr(level + 1);
    }
};

struct SelectQueryNode : QueryNode
{
protected:
    ConditionBodyNode conditionBodyNode_;

public:
    SelectQueryNode() {}
    SelectQueryNode(Command command, const ConditionBodyNode &conditionBodyNode, const ReprNode &reprNode) : QueryNode{command, reprNode}, conditionBodyNode_(conditionBodyNode) {}

    std::string repr(int level) override
    {
        return std::string{"GQLInsertQuery"} +
               "\n├─GQLCommand " + std::to_string(command_) +
               "\n├─" + conditionBodyNode_.repr(level + 1) +
               "\n└─" + reprNode_.repr(level + 1);
    }
};

struct DeleteQueryNode : QueryNode
{
protected:
    ConditionBodyNode conditionBodyNode_;

public:
    DeleteQueryNode() {}
    DeleteQueryNode(Command command, const ConditionBodyNode &conditionBodyNode, const ReprNode &reprNode) : QueryNode{command, reprNode}, conditionBodyNode_(conditionBodyNode) {}
};

struct UpdateQueryNode : QueryNode
{
protected:
    ConditionBodyNode conditionBodyNode_;
    EntityNode entityNode_;

public:
    UpdateQueryNode() {}
    UpdateQueryNode(Command command, const ConditionBodyNode &conditionBodyNode, const EntityNode &entityNode, const ReprNode &reprNode) : QueryNode{command, reprNode}, conditionBodyNode_(conditionBodyNode), entityNode_(entityNode) {}
};

/* --------------------------------------------------------------- */