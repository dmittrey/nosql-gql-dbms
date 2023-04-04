#include <string>
#include <vector>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/vector.hpp>

namespace Query
{
	enum class JsonType
	{
		TYPE_INT32 = 0,
		TYPE_DOUBLE,
		TYPE_STRING,
		TYPE_BOOL,
		TYPE_OBJECT
	};

	class Json
	{
	public:
		JsonType type_;
		std::string key_;
		int32_t int32_val_ = 0;
		double double_val_;
		std::string string_val_;
		bool bool_val_;
		struct Json *dad_ = nullptr;
		struct Json *bro_ = nullptr;
		struct Json *son_ = nullptr;

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

		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive &ar,
					   const unsigned int version)
		{
			ar &BOOST_SERIALIZATION_NVP(type_);
			ar &BOOST_SERIALIZATION_NVP(key_);
			ar &BOOST_SERIALIZATION_NVP(int32_val_);
			ar &BOOST_SERIALIZATION_NVP(double_val_);
			ar &BOOST_SERIALIZATION_NVP(string_val_);
			ar &BOOST_SERIALIZATION_NVP(bool_val_);
			ar &BOOST_SERIALIZATION_NVP(dad_);
			ar &BOOST_SERIALIZATION_NVP(bro_);
			ar &BOOST_SERIALIZATION_NVP(son_);
		}
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

	class ConditionalItem
	{
	public:
		JsonType type_;
		std::string key_;
		Cmp cmp_;
		int32_t int32_val_;
		float double_val_;
		std::string string_val_;
		bool bool_val_;

		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive &ar,
					   const unsigned int version)
		{
			ar &BOOST_SERIALIZATION_NVP(type_);
			ar &BOOST_SERIALIZATION_NVP(key_);
			ar &BOOST_SERIALIZATION_NVP(cmp_);
			ar &BOOST_SERIALIZATION_NVP(int32_val_);
			ar &BOOST_SERIALIZATION_NVP(double_val_);
			ar &BOOST_SERIALIZATION_NVP(string_val_);
			ar &BOOST_SERIALIZATION_NVP(bool_val_);
		}
	};

	class Conditional
	{
	public:
		std::vector<ConditionalItem*> conditionals_;

		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive &ar,
					   const unsigned int version)
		{
			ar &BOOST_SERIALIZATION_NVP(conditionals_);
		}
	};

	enum class QueryType
	{
		INSERT = 0,
		READ,
		UPDATE,
		DELETE
	};

	class Request
	{
	public:
		QueryType type;
		std::string type_name;
		Conditional *query;
		Json *json;

		friend class boost::serialization::access;
		template <class Archive>
		void serialize(Archive &ar,
					   const unsigned int version)
		{
			ar &BOOST_SERIALIZATION_NVP(type);
			ar &BOOST_SERIALIZATION_NVP(type_name);
			ar &BOOST_SERIALIZATION_NVP(query);
			ar &BOOST_SERIALIZATION_NVP(json);
		}
	};
}