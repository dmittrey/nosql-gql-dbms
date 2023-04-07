#pragma once

#include <string>
#include <vector>

#include "network/cmp.hpp"
#include "network/type.hpp"
#include "network/command.hpp"

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
	};

	struct Conditional
	{
		std::vector<ConditionalItem *> conditionals_;
	};

	struct Request
	{
		CommandType type_;
		std::string type_name_;
		Conditional *query_ = nullptr;
		Json *json_ = nullptr;
	};
}