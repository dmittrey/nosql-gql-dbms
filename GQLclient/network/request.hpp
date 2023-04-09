#pragma once

#include <string>
#include <vector>
#include <ostream>

#include "dbms.pb.h"

using dbms::Cmp;
using dbms::JsonType;
using dbms::OperationType;

namespace Network
{
	struct Json
	{
		JsonType type_;
		std::string key_;
		int32_t int32_val_ = 0;
		float float_val_;
		std::string string_val_;
		bool bool_val_;
		Json *dad_ = nullptr;
		Json *bro_ = nullptr;
		Json *son_ = nullptr;

		Json() {}

		Json(const dbms::Json &json) : type_(json.type()),
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

		operator dbms::Json *() const
		{
			dbms::Json *protoJson = new dbms::Json;
			protoJson->set_type(type_);
			protoJson->set_key(key_);
			protoJson->set_int32_val(int32_val_);
			protoJson->set_float_val(float_val_);
			protoJson->set_string_val(string_val_);
			protoJson->set_bool_val(bool_val_);
			if (dad_ != nullptr)
				protoJson->set_allocated_dad(*dad_);
			if (bro_ != nullptr)
				protoJson->set_allocated_bro(*bro_);
			if (son_ != nullptr)
				protoJson->set_allocated_son(*son_);
			return protoJson;
		}

		friend std::ostream &operator<<(std::ostream &out, const Json &json)
		{
			std::cout << "Key:\t" << json.key_ << "\t";
			switch (json.type_)
			{
			case JsonType::TYPE_INT32:
				std::cout << "Value:\t" << json.int32_val_ << std::endl;
				break;
			case JsonType::TYPE_BOOL:
				std::cout << "Value:\t" << json.bool_val_ << std::endl;
				break;
			case JsonType::TYPE_FLOAT:
				std::cout << "Value:\t" << json.float_val_ << std::endl;
				break;
			case JsonType::TYPE_STRING:
				std::cout << "Value:\t" << json.string_val_ << std::endl;
				break;
			case JsonType::TYPE_OBJECT:
				std::cout << "OBJECT" << std::endl;
				break;
			default:
				break;
			}
			if (json.bro_ != nullptr)
				std::cout << *json.bro_ << std::endl;
			if (json.son_ != nullptr)
				std::cout << *json.son_ << std::endl;
			return out;
		}

		void addBro(struct Json &&rhs)
		{
			struct Json *cur = this;
			while (cur->bro_ != nullptr)
			{
				cur = cur->bro_;
			}

			cur->bro_ = new Json(rhs);
		}

		void addBro(struct Json *rhs)
		{
			struct Json *cur = this;
			while (cur->bro_ != nullptr)
			{
				cur = cur->bro_;
			}
			cur->bro_ = rhs;
		}

		void addSon(struct Json *rhs)
		{
			if (son_ != nullptr)
			{
				son_->addBro(rhs);
			}
			else
			{
				son_ = rhs;
			}
		}
	};

	struct ConditionalItem
	{
		JsonType type_;
		std::vector<std::string> key_;
		Cmp cmp_;
		int32_t int32_val_;
		float float_val_;
		std::string string_val_;
		bool bool_val_;

		operator dbms::ConditionalItem *() const
		{
			dbms::ConditionalItem *cond_itm = new dbms::ConditionalItem;
			cond_itm->set_type(type_);
			for (auto &i : key_)
				cond_itm->add_key(i);
			cond_itm->set_cmp(cmp_);
			cond_itm->set_int32_val(int32_val_);
			cond_itm->set_float_val(float_val_);
			cond_itm->set_string_val(string_val_);
			cond_itm->set_bool_val(bool_val_);
			return cond_itm;
		}
	};

	struct Conditional
	{
		std::vector<ConditionalItem *> conditionals_;

		operator dbms::Conditional *() const
		{
			dbms::Conditional *cond = new dbms::Conditional;
			for (auto &i : conditionals_)
			{
				cond->mutable_conditionals()->AddAllocated(*i);
			}
			return cond;
		}
	};

	struct Request
	{
		OperationType type_;
		std::string type_name_;
		Conditional *query_ = nullptr;
		Json *json_ = nullptr;

		operator dbms::OperationRequest() const
		{
			dbms::OperationRequest request;
			request.set_operationtype(type_);
			request.set_typename_(type_name_);
			if (query_ != nullptr)
				request.set_allocated_query(*query_);
			if (json_ != nullptr)
				request.set_allocated_json(*json_);
			return request;
		}
	};
}