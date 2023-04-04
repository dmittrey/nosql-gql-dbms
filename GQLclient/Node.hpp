#pragma once

#include <fstream>
#include <iostream>
#include <boost/serialization/utility.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "query.hpp"

using namespace Query;

enum Command
{
    INSERT,
    DELETE,
    SELECT,
    UPDATE
};

struct Node
{
    std::string offset(int level) { return std::string(level, '\t'); }

    virtual std::string repr(int level) { return "Node"; };
};

/* ---------------------- FIELD NODES ---------------------- */

struct FieldNode : Node
{
    JsonType type_;
    std::string name_;
    std::string str_value_;
    int32_t int_value_;
    double double_value_;
    bool bool_value_;

    FieldNode() {}
    FieldNode(std::string &name) : name_(name) {}
    FieldNode(std::string &name, std::string &value) : name_(name), str_value_(value), type_(JsonType::TYPE_STRING) {}
    FieldNode(std::string &name, int32_t &value) : name_(name), int_value_(value), type_(JsonType::TYPE_INT32) {}
    FieldNode(std::string &name, double &value) : name_(name), double_value_(value), type_(JsonType::TYPE_DOUBLE) {}
    FieldNode(std::string &name, bool &value) : name_(name), bool_value_(value), type_(JsonType::TYPE_BOOL) {}

    std::string repr(int level) override
    {
        std::string str = "GQLField name: " + name_ + "\tvalue: ";
        switch (type_)
        {
        case JsonType::TYPE_STRING:
            str += str_value_;
            break;
        case JsonType::TYPE_INT32:
            str += std::to_string(int_value_);
            break;
        case JsonType::TYPE_DOUBLE:
            str += std::to_string(double_value_);
            break;
        case JsonType::TYPE_BOOL:
            str += std::to_string(bool_value_);
            break;
        default:
            break;
        }

        return str;
    }

    Json *toJson()
    {
        Json *json = new Json;
        json->key_ = name_;
        json->type_ = type_;
        json->string_val_ = str_value_;
        json->int32_val_ = int_value_;
        json->double_val_ = double_value_;
        json->bool_val_ = bool_value_;
        return json;
    }
};

struct FieldListNode : public Node
{
    std::vector<FieldNode> fields = {};

    void add(FieldNode &field) { fields.push_back(field); }

    std::string repr(int level) override
    {
        std::string result = "GQLFieldList";
        for (auto i : fields)
        {
            result += "\n" + offset(level) + "└─" + i.repr(level);
        }
        return result;
    }

    Json *toJson()
    {
        Json *f_son = nullptr;
        for (auto &sonFieldNode : fields)
        {
            if (f_son == nullptr)
                f_son = sonFieldNode.toJson();
            else
                f_son->addBro(sonFieldNode.toJson());
        }
        return f_son;
    }
};

/* ---------------------- CONDITIONAL NODES ---------------------- */

struct PropertyNode : FieldNode
{
    Cmp cmp_;

    PropertyNode() {}
    PropertyNode(std::string &name, Cmp cmp, std::string &value) : FieldNode{name, value}, cmp_(cmp) {}
    PropertyNode(std::string &name, Cmp cmp, int32_t &value) : FieldNode{name, value}, cmp_(cmp) {}
    PropertyNode(std::string &name, Cmp cmp, double &value) : FieldNode{name, value}, cmp_(cmp) {}
    PropertyNode(std::string &name, Cmp cmp, bool &value) : FieldNode{name, value}, cmp_(cmp) {}

    std::string repr(int level) override
    {
        std::string str = "GQLProperty name: " + name_ + "\tOperation: " + std::to_string(cmp_) + "\tvalue: ";
        switch (type_)
        {
        case JsonType::TYPE_STRING:
            str += str_value_;
            break;
        case JsonType::TYPE_INT32:
            str += std::to_string(int_value_);
            break;
        case JsonType::TYPE_DOUBLE:
            str += std::to_string(double_value_);
            break;
        case JsonType::TYPE_BOOL:
            str += std::to_string(bool_value_);
            break;
        default:
            break;
        }

        return str;
    }

    ConditionalItem *toConditionalItem()
    {
        ConditionalItem *item = new ConditionalItem;
        item->key_ = name_;
        item->type_ = type_;
        item->cmp_ = cmp_;
        item->string_val_ = str_value_;
        item->int32_val_ = int_value_;
        item->double_val_ = double_value_;
        item->bool_val_ = bool_value_;
        return item;
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

    std::vector<ConditionalItem *> toConditionalItemVector()
    {
        std::vector<ConditionalItem *> cond_items{};
        for (auto &i : fields)
        {
            cond_items.push_back(i.toConditionalItem());
        }
        return cond_items;
    }
};

/* ---------------------- ALIAS NODES ---------------------------- */

struct ConditionNode : Node
{
    PropertyListNode propertyListNode_;

    ConditionNode() {}
    ConditionNode(const PropertyListNode &propertyListNode) : propertyListNode_(propertyListNode) {}

    std::string repr(int level) override
    {
        return std::string{"GQLCondition"} +
               "\n" + offset(level) + "└─" + propertyListNode_.repr(level + 1);
    }

    Conditional *toConditional()
    {
        return new Conditional{propertyListNode_.toConditionalItemVector()};
    }
};

struct EntityNode : Node
{
    bool is_root_ = false;
    std::string nodeName_;
    FieldListNode fieldListNode_;

    EntityNode() {}
    EntityNode(const FieldListNode &fieldListNode) : is_root_(true), fieldListNode_(fieldListNode) {}
    EntityNode(const std::string &nodeName, const FieldListNode &fieldListNode) : nodeName_(nodeName), fieldListNode_(fieldListNode) {}

    std::string repr(int level) override
    {
        return std::string{"GQLEntity"} +
               "\n" + offset(level) + "├─" + "GQLEntityIsRoot " + std::to_string(is_root_) +
               "\n" + offset(level) + "├─" + "GQLEntityName " + nodeName_ +
               "\n" + offset(level) + "└─" + fieldListNode_.repr(level + 1);
    }

    std::pair<bool, Json *> toJson()
    {
        Json *rootJson = new Json;
        rootJson->type_ = JsonType::TYPE_OBJECT;
        rootJson->key_ = nodeName_;

        rootJson->addSon(fieldListNode_.toJson());
        return {is_root_, rootJson};
    }
};

struct EntityListNode : Node
{
    std::vector<EntityNode> entities;

    void add(const EntityNode &node) { entities.push_back(node); }

    std::string repr(int level) override
    {
        std::string result = std::string{"GQLEntityList"};
        for (auto i : entities)
        {
            result += "\n" + offset(level) + "└─" + i.repr(level + 1);
        }
        return result;
    }

    Json *toJson()
    {
        Json *rootJson;
        for (auto &entity : entities)
        {
            std::pair<bool, Json *> entityJson = entity.toJson();
            if (entityJson.first == true)
                rootJson = entityJson.second;
        }

        for (auto &entity : entities)
        {
            std::pair<bool, Json *> entityJson = entity.toJson();
            if (entityJson.first == false)
                rootJson->addSon(entityJson.second);
        }
        return rootJson;
    }
};

/* ---------------------- BODY NODES ----------------------------- */

struct ConditionBodyNode : Node
{
    std::string typeName_;
    ConditionNode conditionNode_;

    ConditionBodyNode() {}
    ConditionBodyNode(const std::string &typeName, const ConditionNode &conditionNode) : typeName_(typeName), conditionNode_(conditionNode) {}

    std::string repr(int level) override
    {
        return std::string{"GQLConditionBody"} +
               "\n" + offset(level) + "├─GQLWord " + typeName_ +
               "\n" + offset(level) + "└─" + conditionNode_.repr(level + 1);
    }

    std::pair<std::string, Conditional *> toConditionalWithType()
    {
        return {typeName_, conditionNode_.toConditional()};
    }
};

struct EntityBodyNode : Node
{
    std::string typeName_;
    EntityListNode entityList_;

    EntityBodyNode() {}
    EntityBodyNode(const std::string &typeName, const EntityListNode &entityList) : typeName_(typeName), entityList_(entityList) {}

    std::string repr(int level) override
    {
        return std::string{"GQLEntityBody"} +
               "\n" + offset(level) + "├─GQLWord " + typeName_ +
               "\n" + offset(level) + "└─" + entityList_.repr(level + 1);
    }

    std::pair<std::string, Json *> toJsonWithType()
    {
        return {typeName_, entityList_.toJson()};
    }
};

/* ---------------------- QUERY NODES ----------------------------- */

struct QueryNode : Node
{
    Command command_;

    QueryNode() {}
    QueryNode(Command command) : command_(command) {}

    std::string repr(int level) override
    {
        return "Hello from query node!";
    }

    virtual Request *toRequest() { return nullptr; };
};

struct InsertQueryNode : QueryNode
{
    EntityBodyNode entityBodyNode_;

    InsertQueryNode() {}
    InsertQueryNode(Command command, const EntityBodyNode &entityBodeNode) : QueryNode{command}, entityBodyNode_(entityBodeNode) {}

    std::string repr(int level) override
    {
        return std::string{"GQLInsertQuery"} +
               "\n├─GQLCommand " + std::to_string(command_) +
               "\n└─" + entityBodyNode_.repr(level + 1);
    }

    Request *toRequest() override
    {
        std::pair<std::string, Json *> typeWithJson = entityBodyNode_.toJsonWithType();

        std::ofstream ofs("file.xml");
        unsigned int flags = boost::archive::no_header;
        boost::archive::xml_oarchive oa(ofs, flags);
        oa << BOOST_SERIALIZATION_NVP(typeWithJson);

        Request *request = new Request;
        request->type = QueryType::INSERT;
        request->type_name = typeWithJson.first;
        request->json = typeWithJson.second;

        return request;
    }
};

struct SelectQueryNode : QueryNode
{
    ConditionBodyNode conditionBodyNode_;

    SelectQueryNode() {}
    SelectQueryNode(Command command, const ConditionBodyNode &conditionBodyNode) : QueryNode{command}, conditionBodyNode_(conditionBodyNode) {}

    std::string repr(int level) override
    {
        return std::string{"GQLSelectQuery"} +
               "\n├─GQLCommand " + std::to_string(command_) +
               "\n└─" + conditionBodyNode_.repr(level + 1);
    }

    Request *toRequest() override
    {
        std::pair<std::string, Conditional *> typeWithConditional = conditionBodyNode_.toConditionalWithType();

        Request *request = new Request{};
        request->type = QueryType::INSERT;
        request->type_name = typeWithConditional.first;
        request->query = typeWithConditional.second;

        return request;
    }
};

struct DeleteQueryNode : QueryNode
{
    ConditionBodyNode conditionBodyNode_;

    DeleteQueryNode() {}
    DeleteQueryNode(Command command, const ConditionBodyNode &conditionBodyNode) : QueryNode{command}, conditionBodyNode_(conditionBodyNode) {}

    std::string repr(int level) override
    {
        return std::string{"GQLDeleteQuery"} +
               "\n├─GQLCommand " + std::to_string(command_) +
               "\n└─" + conditionBodyNode_.repr(level + 1);
    }

    Request *toRequest() override
    {
        std::pair<std::string, Conditional *> typeWithConditional = conditionBodyNode_.toConditionalWithType();

        Request *request = new Request{};
        request->type = QueryType::INSERT;
        request->type_name = typeWithConditional.first;
        request->query = typeWithConditional.second;

        return request;
    }
};

struct UpdateQueryNode : QueryNode
{
    ConditionBodyNode conditionBodyNode_;
    EntityBodyNode entityBodyNode_;

    UpdateQueryNode() {}
    UpdateQueryNode(Command command, const ConditionBodyNode &conditionBodyNode, const EntityBodyNode &entityBodyNode) : QueryNode{command}, conditionBodyNode_(conditionBodyNode), entityBodyNode_(entityBodyNode) {}

    std::string repr(int level) override
    {
        return std::string{"GQLUpdateQuery"} +
               "\n├─GQLCommand " + std::to_string(command_) +
               "\n├─" + conditionBodyNode_.repr(level + 1) +
               "\n└─" + entityBodyNode_.repr(level + 1);
    }

    Request *toRequest() override
    {
        std::pair<std::string, Conditional *> typeWithConditional = conditionBodyNode_.toConditionalWithType();
        std::pair<std::string, Json *> typeWithJson = entityBodyNode_.toJsonWithType();

        Request *request = new Request{};
        request->type = QueryType::INSERT;
        request->type_name = typeWithConditional.first;
        request->query = typeWithConditional.second;
        request->json = typeWithJson.second;

        return request;
    }
};