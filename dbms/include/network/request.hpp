#pragma once

#include <string>
#include <vector>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/vector.hpp>

extern "C"
{
#include "user/command.h"
#include "json/json.h"
}

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