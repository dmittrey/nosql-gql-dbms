#pragma once

#include <string>
#include <vector>

#include "dbms.pb.h"

extern "C"
{
#include "user/command.h"
#include "json/json.h"
}

using dbms::OperationRequest;

namespace Network
{
	struct Json
	{
		Json_type type_;
		std::string key_;
		int32_t int32_val_ = 0;
		float float_val_;
		std::string string_val_;
		bool bool_val_;
		Json *dad_ = nullptr;
		Json *bro_ = nullptr;
		Json *son_ = nullptr;

		Json(const ::Json &json) : type_(json.type), key_(json.key->val)
		{
			if (type_ == TYPE_STRING)
			{
				string_val_ = std::string(json.value.string_val->val, json.value.string_val->cnt);
			}
			else if (type_ != TYPE_OBJECT)
			{
				int32_val_ = json.value.int32_val;
				float_val_ = json.value.float_val;
				bool_val_ = json.value.bool_val;
			}

			if (json.dad != NULL)
				dad_ = new Json(*json.dad);
			if (json.bro != NULL)
				bro_ = new Json(*json.bro);
			if (json.son != NULL)
				son_ = new Json(*json.son);
		}

		Json(const dbms::Json &json) : type_(static_cast<Json_type>(json.type())),
									   key_(json.key()),
									   int32_val_(json.int32_val()),
									   float_val_(json.float_val()),
									   string_val_(json.string_val()),
									   bool_val_(json.bool_val())
		{
			if (json.has_dad())
				dad_ = new Json(json.dad());
			if (json.has_bro())
				bro_ = new Json(json.bro());
			if (json.has_son())
				son_ = new Json(json.son());
		}

		operator ::Json *() const
		{
			::Json *json = json_new();
			json_ctor(json, type_, key_.c_str(), key_.length());
			if (json->type == TYPE_STRING)
			{
				json->value.string_val = string_new();
				string_ctor(json->value.string_val, string_val_.c_str(), string_val_.length());
			}
			else if (json->type != TYPE_OBJECT)
			{
				json->value.int32_val = int32_val_;
				json->value.float_val = float_val_;
				json->value.bool_val = bool_val_;
			}
			if (dad_ != nullptr)
				json->dad = *dad_;
			if (bro_ != nullptr)
				json->bro = *bro_;
			if (son_ != nullptr)
				json->son = *son_;
			return json;
		}

		operator dbms::Json *() const
		{
			dbms::Json *json = new dbms::Json;
			json->set_type(static_cast<dbms::JsonType>(type_));
			json->set_key(key_);
			json->set_int32_val(int32_val_);
			json->set_float_val(float_val_);
			json->set_string_val(string_val_);
			json->set_bool_val(bool_val_);
			if (dad_ != nullptr)
				json->set_allocated_dad(*dad_);
			if (bro_ != nullptr)
				json->set_allocated_bro(*bro_);
			if (son_ != nullptr)
				json->set_allocated_son(*son_);
			return json;
		}
	};

	struct ConditionalItem
	{
		Json_type type_;
		std::vector<std::string> key_;
		Cmp cmp_;
		int32_t int32_val_;
		float float_val_;
		std::string string_val_;
		bool bool_val_;

		ConditionalItem(dbms::ConditionalItem conditionalItem) : type_(static_cast<Json_type>(conditionalItem.type())),
																 cmp_(static_cast<Cmp>(conditionalItem.cmp())),
																 int32_val_(conditionalItem.int32_val()),
																 float_val_(conditionalItem.float_val()),
																 string_val_(conditionalItem.string_val()),
																 bool_val_(conditionalItem.bool_val())
		{
			for (size_t i = 0; i < conditionalItem.mutable_key()->size(); i++)
				key_.push_back(conditionalItem.key(i));
		}

		operator Query_item *() const
		{
			Query_item *q_itm = query_item_new();
			query_item_ctor(q_itm, type_, toListPath(), cmp_);
			if (q_itm->type == TYPE_STRING)
			{
				q_itm->query_val.string_val = string_new();
				string_ctor(q_itm->query_val.string_val, string_val_.c_str(), string_val_.length());
			}
			else if (q_itm->type != TYPE_OBJECT)
			{
				q_itm->query_val.int32_val = int32_val_;
				q_itm->query_val.float_val = float_val_;
				q_itm->query_val.bool_val = bool_val_;
			}
			return q_itm;
		}

	private:
		List_Path *toListPath() const
		{
			List_Path *list_path_ptr = list_Path_new();
			for (auto &i : key_)
			{
				String *path_name = string_new();
				string_ctor(path_name, i.c_str(), i.length());
				PATH(path_name);
				list_Path_add(list_path_ptr, path_name_path);
			}
			return list_path_ptr;
		}
	};

	struct Conditional
	{
		std::vector<ConditionalItem *> conditionals_;

		Conditional(dbms::Conditional conditional)
		{
			conditionals_ = std::vector<ConditionalItem *>();
			for (size_t i = 0; i < conditional.conditionals_size(); i++)
				conditionals_.push_back(new ConditionalItem(conditional.conditionals(i)));
		}

		operator ::Query *()
		{
			Query *query = query_new();
			for (auto &i : conditionals_)
			{
				query_item_add(query, *i);
			}
			return query;
		}
	};

	struct Request
	{
		CommandType type_;
		std::string type_name_;
		Conditional *query_ = nullptr;
		Json *json_ = nullptr;

		Request(const dbms::OperationRequest &operationRequest) : type_(static_cast<CommandType>(operationRequest.operationtype())),
																  type_name_(operationRequest.typename_()),
																  query_(new Conditional(operationRequest.query())),
																  json_(new Json(operationRequest.json())) {}

		operator Command() const
		{
			Command command;
			command.type = type_;
			command.type_name = string_new();
			string_ctor(command.type_name, type_name_.c_str(), type_name_.length());
			if (query_ != nullptr)
				command.query = *query_;
			if (json_ != nullptr)
				command.json = *json_;
			return command;
		}
	};
}