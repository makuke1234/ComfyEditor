#pragma once

#include <exception>
#include <type_traits>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>

#include <cstdint>
#include <cstring>
#include <cassert>


namespace jsonlite2
{
	auto constexpr MAX_NUMBERLEN{ 32 };

	template<class T>
	static constexpr const T & var_max(const T & val1) noexcept
	{
		return val1;
	}
	template<class T, class ... Args>
	static constexpr const T & var_max(const T & val1, const T & val2, const Args & ... args) noexcept
	{
		return var_max((val1 < val2) ? val2 : val1, args...);
	}
	template<class T>
	static constexpr const T & var_min(const T & val1) noexcept
	{
		return val1;
	}
	template<class T, class ... Args>
	static constexpr const T & var_min(const T & val1, const T & val2, const Args & ... args) noexcept
	{
		return var_min((val1 < val2) ? val1 : val2, args...);
	}

	enum class error : std::uint8_t
	{
		ok,
		unknown,
		mem,
		invalidChar,
		moreThan1Main,
		noKey,
		noTerminatingQuote,
		invalidValueSeparator,
		noValueSeparator,
		noValue,
		noSeparator,
		excessiveArrayTerminator,
		excessiveObjectTerminator,
		noArrayTerminator,
		noObjectTerminator,
		invalidTerminator,
		multipleDecimalPoints,
		noDigitAfterDecimal,
	};

	constexpr const char * g_jsonErrors[]
	{
		"All OK",
		"Unknown error!",
		"Memory allocation error!",
		"Invalid character!",
		"More than 1 main value in file!",
		"No key in object!",
		"No terminating quote for string!",
		"Invalid value separator in object!",
		"No value separator in object!",
		"No value in object's key-value pair!",
		"No spearator between values!",
		"Excessive array terminator!",
		"Excessive object terminator!",
		"No terminating ']'!",
		"No terminating '}'!",
		"Invalid terminator!",
		"Multiple decimal points '.' in number!",
		"No digits after decimal point!",
	};

	class jsonKeyValue;
	class jsonArray;
	class jsonObject;

	class json;


	class jsonValue
	{
	public:
		enum class type : std::uint8_t
		{
			null,
			string,
			boolean,
			number,
			array,
			object
		};

	private:
		friend class jsonKeyValue;
		friend class jsonArray;
		friend class jsonObject;
		friend class json;

		type m_type{ type::null };
		union data
		{
			std::string * string;
			bool boolean;
			double number;
			jsonArray * array;
			jsonObject * object;
		} m_d;

		static inline jsonValue p_parse(const char *& it, const char * end);
		inline std::string p_dump(std::size_t depth, bool nonobj = false) const;

	public:
		jsonValue() noexcept = default;
		jsonValue(std::string * str) noexcept
			: m_type(type::string)
		{
			this->m_d.string = str;
		}
		jsonValue(const std::string & str)
			: m_type(type::string)
		{
			this->m_d.string = new std::string(str);
		}
		jsonValue(bool boolean) noexcept
			: m_type(type::boolean)
		{
			this->m_d.boolean = boolean;
		}
		jsonValue(double number) noexcept
			: m_type(type::number)
		{
			this->m_d.number = number;
		}
		jsonValue(jsonArray * arr) noexcept;
		jsonValue(const jsonArray & arr);
		jsonValue(jsonObject * obj) noexcept;
		jsonValue(const jsonObject & obj);

		jsonValue(const jsonValue & other);
		jsonValue(jsonValue && other) noexcept;
		jsonValue & operator=(const jsonValue & other);
		jsonValue & operator=(jsonValue && other) noexcept;

		~jsonValue() noexcept;


		constexpr type getType() const noexcept
		{
			return this->m_type;
		}
		std::string & getString()
		{
			if (this->m_type != type::string)
			{
				throw std::runtime_error("Invalid JSON type!");
			}
			return *this->m_d.string;
		}
		const std::string & getString() const
		{
			if (this->m_type != type::string)
			{
				throw std::runtime_error("Invalid JSON type!");
			}
			return *this->m_d.string;
		}
		bool & getBoolean()
		{
			if (this->m_type != type::boolean)
			{
				throw std::runtime_error("Invalid JSON type!");
			}
			return this->m_d.boolean;
		}
		const bool & getBoolean() const
		{
			if (this->m_type != type::boolean)
			{
				throw std::runtime_error("Invalid JSON type!");
			}
			return this->m_d.boolean;
		}
		double & getNumber()
		{
			if (this->m_type != type::number)
			{
				throw std::runtime_error("Invalid JSON type!");
			}
			return this->m_d.number;
		}
		const double & getNumber() const
		{
			if (this->m_type != type::number)
			{
				throw std::runtime_error("Invalid JSON type!");
			}
			return this->m_d.number;
		}
		jsonArray & getArray()
		{
			if (this->m_type != type::array)
			{
				throw std::runtime_error("Invalid JSON type!");
			}
			return *this->m_d.array;
		}
		const jsonArray & getArray() const
		{
			if (this->m_type != type::array)
			{
				throw std::runtime_error("Invalid JSON type!");
			}
			return *this->m_d.array;
		}
		jsonObject & getObject()
		{
			if (this->m_type != type::object)
			{
				throw std::runtime_error("Invalid JSON type!");
			}
			return *this->m_d.object;
		}
		const jsonObject & getObject() const
		{
			if (this->m_type != type::object)
			{
				throw std::runtime_error("Invalid JSON type!");
			}
			return *this->m_d.object;
		}

		jsonKeyValue & operator[](const std::string & key);
		const jsonKeyValue & operator[](const std::string & key) const;
		jsonKeyValue & at(const std::string & key);
		const jsonKeyValue & at(const std::string & key) const;

		jsonValue & operator[](std::size_t idx);
		const jsonValue & operator[](std::size_t idx) const;
		jsonValue & at(std::size_t idx);
		const jsonValue & at(std::size_t idx) const;

		std::string dump() const
		{
			return this->p_dump(0);
		}

	};

	class jsonArray
	{
	private:
		friend class jsonValue;

		std::vector<jsonValue> m_vals;

		static inline jsonArray p_parse(const char *& it, const char * end);
		inline std::string p_dump(std::size_t depth) const;

	public:

		jsonValue & operator[](std::size_t idx) noexcept
		{
			return this->m_vals[idx];
		}
		const jsonValue & operator[](std::size_t idx) const noexcept
		{
			return this->m_vals[idx];
		}
		jsonValue & at(std::size_t idx)
		{
			return this->m_vals.at(idx);
		}
		const jsonValue & at(std::size_t idx) const
		{
			return this->m_vals.at(idx);
		}

		std::string dump() const
		{
			return this->p_dump(0);
		}

	};

	class jsonKeyValue
	{
	private:
		friend class jsonObject;

		std::string m_key;
		jsonValue m_value;
		bool m_empty{ true };

		static inline jsonKeyValue p_parse(const char *& it, const char * end);
		inline std::string p_dump(std::size_t depth) const;

	public:
		jsonKeyValue() noexcept = default;

		jsonKeyValue(const std::string & key, const jsonValue & value = {})
			: m_key(key), m_value(value), m_empty(false)
		{
		}
		jsonKeyValue(std::string && key, jsonValue && value = {})
			: m_key(std::move(key)), m_value(std::move(value)), m_empty(false)
		{
		}

		constexpr bool empty() const noexcept
		{
			return this->m_empty;
		}

		constexpr const std::string & key() const noexcept
		{
			return this->m_key;
		}
		jsonValue & get() noexcept
		{
			return this->m_value;
		}
		constexpr const jsonValue & get() const noexcept
		{
			return this->m_value;
		}
		operator jsonValue & () noexcept
		{
			return this->m_value;
		}
		constexpr operator const jsonValue & () const noexcept
		{
			return this->m_value;
		}

		jsonValue * operator->()
		{
			return &this->m_value;
		}
		constexpr const jsonValue * operator->() const noexcept
		{
			return &this->m_value;
		}

		std::string dump() const
		{
			return this->p_dump(0);
		}

	};

	class jsonObject
	{
	private:
		friend class jsonValue;

		std::vector<jsonKeyValue> m_keyvalues;
		std::unordered_map<std::string, std::size_t> m_map;

		static inline jsonObject p_parse(const char *& it, const char * end);
		inline std::string p_dump(std::size_t depth) const;

	public:

		jsonKeyValue & operator[](const std::string & key)
		{
			auto it = this->m_map.find(key);
			if (it == this->m_map.end())
			{
				this->m_map.emplace(key, this->m_keyvalues.size());
				this->m_keyvalues.emplace_back(key);
			}
			return this->m_keyvalues[it->second];
		}
		const jsonKeyValue & operator[](const std::string & key) const
		{
			auto it = this->m_map.find(key);
			if (it == this->m_map.end())
			{
				throw std::runtime_error("Invalid JSON key!");
			}
			return this->m_keyvalues[it->second];
		}
		jsonKeyValue & at(const std::string & key)
		{
			auto it = this->m_map.find(key);
			if (it == this->m_map.end())
			{
				throw std::runtime_error("Invalid JSON key!");
			}
			return this->m_keyvalues[it->second];
		}
		const jsonKeyValue & at(const std::string & key) const
		{
			auto it = this->m_map.find(key);
			if (it == this->m_map.end())
			{
				throw std::runtime_error("Invalid JSON key!");
			}
			return this->m_keyvalues[it->second];
		}

		bool remove(const std::string & key) noexcept
		{
			auto it = this->m_map.find(key);
			if (it == this->m_map.end())
			{
				return false;
			}
			this->m_keyvalues[it->second].~jsonKeyValue();//.reset();
			this->m_map.erase(key);
			return true;
		}

		std::string dump() const
		{
			return this->p_dump(0);
		}

	};

	inline jsonValue::jsonValue(jsonArray * arr) noexcept
		: m_type(type::array)
	{
		this->m_d.array = arr;
	}
	inline jsonValue::jsonValue(const jsonArray & arr)
		: m_type(type::array)
	{
		this->m_d.array = new jsonArray(arr);
	}
	inline jsonValue::jsonValue(jsonObject * obj) noexcept
		: m_type(type::object)
	{
		this->m_d.object = obj;
	}
	inline jsonValue::jsonValue(const jsonObject & obj)
		: m_type(type::object)
	{
		this->m_d.object = new jsonObject(obj);
	}

	inline jsonValue::jsonValue(const jsonValue & other)
		: m_type(other.m_type)
	{
		switch (this->m_type)
		{
		case type::boolean:
			this->m_d.boolean = other.m_d.boolean;
			break;
		case type::number:
			this->m_d.number = other.m_d.number;
			break;
		case type::string:
			this->m_d.string = new std::string(*other.m_d.string);
			break;
		case type::array:
			this->m_d.array = new jsonArray(*other.m_d.array);
			break;
		case type::object:
			this->m_d.object = new jsonObject(*other.m_d.object);
			break;
		}
	}
	inline jsonValue::jsonValue(jsonValue && other) noexcept
		: m_type(other.m_type), m_d(other.m_d)
	{
		other.m_type = type::null;
		other.m_d = { 0 };
	}
	inline jsonValue & jsonValue::operator=(const jsonValue & other)
	{
		this->~jsonValue();
		this->m_type = other.m_type;
		switch (this->m_type)
		{
		case type::boolean:
			this->m_d.boolean = other.m_d.boolean;
			break;
		case type::number:
			this->m_d.number = other.m_d.number;
			break;
		case type::string:
			this->m_d.string = new std::string(*other.m_d.string);
			break;
		case type::array:
			this->m_d.array = new jsonArray(*other.m_d.array);
			break;
		case type::object:
			this->m_d.object = new jsonObject(*other.m_d.object);
			break;
		}
		return *this;
	}
	inline jsonValue & jsonValue::operator=(jsonValue && other) noexcept
	{
		this->~jsonValue();
		this->m_type = other.m_type;
		this->m_d = other.m_d;
		other.m_type = type::null;
		other.m_d = { 0 };
		return *this;
	}

	inline jsonValue::~jsonValue() noexcept
	{
		switch (this->m_type)
		{
		case type::string:
			delete this->m_d.string;
			break;
		case type::array:
			delete this->m_d.array;
			break;
		case type::object:
			delete this->m_d.object;
			break;
		}
		this->m_type = type::null;
	}

	inline jsonKeyValue & jsonValue::operator[](const std::string & key)
	{
		return this->getObject()[key];
	}
	inline const jsonKeyValue & jsonValue::operator[](const std::string & key) const
	{
		return this->getObject()[key];
	}
	inline jsonKeyValue & jsonValue::at(const std::string & key)
	{
		return this->getObject().at(key);
	}
	inline const jsonKeyValue & jsonValue::at(const std::string & key) const
	{
		return this->getObject().at(key);
	}

	inline jsonValue & jsonValue::operator[](std::size_t idx)
	{
		return this->getArray()[idx];
	}
	inline const jsonValue & jsonValue::operator[](std::size_t idx) const
	{
		return this->getArray()[idx];
	}
	inline jsonValue & jsonValue::at(std::size_t idx)
	{
		return this->getArray().at(idx);
	}
	inline const jsonValue & jsonValue::at(std::size_t idx) const
	{
		return this->getArray().at(idx);
	}

	// Value parsing

	inline jsonArray jsonArray::p_parse(const char *& it, const char * end)
	{
		assert(it != nullptr);
		assert(end != nullptr);

		for (; it != end; ++it)
		{
			if (*it == '[')
			{
				++it;
				break;
			}
		}
		jsonArray arr;

		bool ended = false;
		while (it != end)
		{
			switch (*it)
			{
			case ']':
				ended = true;
				/* fall through */
			case '}':
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				++it;
				break;
			default:
				arr.m_vals.emplace_back(jsonValue::p_parse(it, end));
			}
			if (ended)
			{
				break;
			}
		}

		if (!ended)
		{
			throw std::runtime_error(g_jsonErrors[std::uint8_t(error::unknown)]);
		}

		return arr;
	}
	inline jsonValue jsonValue::p_parse(const char *& it, const char * end)
	{
		assert(it != nullptr);
		assert(end != nullptr);

		jsonValue val;

		bool done = false;
		for (; it != end; ++it)
		{
			switch (*it)
			{
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				break;
			case '{':
				val.m_type = type::object;
				val.m_d.object = new jsonObject(jsonObject::p_parse(it, end));
				done = true;
				break;
			case '[':
				val.m_type = type::array;
				val.m_d.array = new jsonArray(jsonArray::p_parse(it, end));
				done = true;
				break;
			case '"':
				++it;
				for (const char * begin = it; it != end; ++it)
				{
					if ((*it == '\\') && ((it + 1) != end))
					{
						++it;
						continue;
					}
					else if (*it == '"')
					{
						val.m_type = type::string;
						val.m_d.string = new std::string(begin, std::size_t(it - begin));
						done = true;
						++it;
						break;
					}
				}
				if (!done)
				{
					throw std::runtime_error(g_jsonErrors[std::uint8_t(error::unknown)]);
				}
				break;
			case 'f':
				if (((end - it) >= 5) && (std::strncmp(it, "false", 5) == 0))
				{
					it += 5;
					val.m_type = type::boolean;
					val.m_d.boolean = false;
					done = true;
				}
				else
				{
					throw std::runtime_error(g_jsonErrors[std::uint8_t(error::unknown)]);
				}
				break;
			case 't':
				if (((end - it) >= 4) && (std::strncmp(it, "true", 4) == 0))
				{
					it += 4;
					val.m_type = type::boolean;
					val.m_d.boolean = true;
					done = true;
				}
				else
				{
					throw std::runtime_error(g_jsonErrors[std::uint8_t(error::unknown)]);
				}
				break;
			case 'n':
				if (((end - it) >= 4) && (std::strncmp(it, "null", 4) == 0))
				{
					it += 4;
					val.m_type = type::null;
					done = true;
				}
				else
				{
					throw std::runtime_error(g_jsonErrors[std::uint8_t(error::unknown)]);
				}
				break;
			default:
				if ((*it >= '0') && (*it <= '9'))
				{
					const char * begin = it;

					for (; it != end; ++it)
					{
						if (((*it >= '0') && (*it <= '9')) || (*it == '.'))
						{
							continue;
						}
						else
						{
							break;
						}
					}
					val.m_type = type::number;
					val.m_d.number = std::strtod(begin, nullptr);
					done = true;
				}
				else
				{
					throw std::runtime_error(g_jsonErrors[std::uint8_t(error::unknown)]);
				}
			}

			if (done)
			{
				break;
			}
		}

		// Find comma
		for (; it != end; ++it)
		{
			switch (*it)
			{
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				break;
			case ',':
				++it;
				/* fall through */
			case ']':
			case '}':
				return val;
			default:
				throw std::runtime_error(g_jsonErrors[std::uint8_t(error::unknown)]);
			}
		}
		return val;
	}
	inline jsonKeyValue jsonKeyValue::p_parse(const char *& it, const char * end)
	{
		assert(it != nullptr);
		assert(end != nullptr);

		jsonKeyValue kv;
		kv.m_empty = false;

		++it;
		for (const char * begin = it; it != end; ++it)
		{
			if ((*it == '\\') && ((it + 1) != end))
			{
				++it;
				continue;
			}
			else if (*it == '"')
			{
				kv.m_key = std::string(begin, std::size_t(it - begin));
				++it;
				break;
			}
		}
		for (; it != end; ++it)
		{
			if (*it == ':')
			{
				++it;
				break;
			}
		}
		kv.m_value = jsonValue::p_parse(it, end);
		return kv;
	}
	inline jsonObject jsonObject::p_parse(const char *& it, const char * end)
	{
		assert(it != nullptr);
		assert(end != nullptr);

		jsonObject obj;
		for (; it != end; ++it)
		{
			if (*it == '{')
			{
				++it;
				break;
			}
		}
		bool ended = false;
		while (it != end)
		{
			switch (*it)
			{
			case '"':
				obj.m_keyvalues.emplace_back(jsonKeyValue::p_parse(it, end));
				obj.m_map.emplace(obj.m_keyvalues.back().m_key, std::size_t(obj.m_keyvalues.size() - 1));
				break;
			case '}':
				ended = true;
				/* fall through */
			case ']':
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				++it;
				break;
			default:
				throw std::runtime_error(g_jsonErrors[std::uint8_t(error::unknown)]);
			}

			if (ended)
			{
				break;
			}
		}
		if (!ended)
		{
			throw std::runtime_error(g_jsonErrors[std::uint8_t(error::unknown)]);
		}

		return obj;
	}

	inline std::string jsonArray::p_dump(std::size_t depth) const
	{
		std::string out(depth, '\t');
		out += "[\n";

		for (size_t i = 0; i < this->m_vals.size(); ++i)
		{
			out += this->m_vals[i].p_dump(depth + 1);
			if (i < (this->m_vals.size() - 1))
			{
				out.back() = ',';
				out += '\n';
			}
		}

		out.append(depth, '\t');
		out += "]\n";

		return out;
	}
	inline std::string jsonValue::p_dump(std::size_t depth, bool nonobj) const
	{
		switch (this->m_type)
		{
		case type::array:
			return this->m_d.array->p_dump(depth);
		case type::object:
			return this->m_d.object->p_dump(depth);
		default:
		{
			std::string out;
			if (!nonobj)
			{
				out.append(depth, '\t');
			}
			switch (this->m_type)
			{
			case type::null:
				out += "null";
				break;
			case type::string:
				out += '"';
				out += *this->m_d.string;
				out += '"';
				break;
			case type::boolean:
				out += (this->m_d.boolean) ? "true" : "false";
				break;
			case type::number:
			{
				char temp[MAX_NUMBERLEN];
				auto stringLen = sprintf_s(temp, MAX_NUMBERLEN, "%.15g", this->m_d.number);

				if (stringLen <= 0)
				{
					throw std::runtime_error("Failed to convert number to string!");
				}
				out.append(temp, stringLen);

				break;
			}
			}
			out += '\n';
			return out;
		}
		}
	}
	inline std::string jsonKeyValue::p_dump(std::size_t depth) const
	{
		std::string out(depth, '\t');

		out += '"';
		out += this->m_key;

		bool nonObj = false;
		switch (this->m_value.m_type)
		{
		case jsonValue::type::array:
		case jsonValue::type::object:
			break;
		default:
			nonObj = true;
		}

		out += nonObj ? "\": " : "\":\n";
		out += this->m_value.p_dump(depth, nonObj);

		return out;
	}
	inline std::string jsonObject::p_dump(std::size_t depth) const
	{
		std::string out(depth, '\t');

		out += "{\n";

		for (size_t i = 0; i < this->m_keyvalues.size(); ++i)
		{
			if (this->m_keyvalues[i].empty())
			{
				continue;
			}
			out += this->m_keyvalues[i].p_dump(depth + 1);
			if (i < (this->m_keyvalues.size() - 1))
			{
				out.back() = ',';
				out += '\n';
			}
		}

		out.append(depth, '\t');
		out += "}\n";

		return out;
	}


	class json
	{
	private:
		jsonValue m_value;

	public:
		json() noexcept = default;
		json(const jsonValue & v)
			: m_value(v)
		{
		}
		json(jsonValue && v) noexcept
			: m_value(std::move(v))
		{
		}

		static inline void p_checkValue(const char *& it, const char * end, error & err) noexcept;
		static inline void p_checkKeyValue(const char *& it, const char * end, error & err) noexcept;
		static inline std::size_t p_checkValues(const char *& it, const char * end, error & err) noexcept;
		static inline void p_checkObject(const char *& it, const char * end, error & err) noexcept;
		static inline void p_checkArray(const char *& it, const char * end, error & err) noexcept;

		static inline error p_check(const char * str, std::size_t len) noexcept
		{
			assert(str != nullptr);

			error err{ error::ok };
			size_t firstLevelObjects = p_checkValues(str, str + len, err);

			if ((err == error::ok) && (firstLevelObjects > 1))
			{
				return error::moreThan1Main;
			}
			return err;
		}
		static inline json p_parse(const char * str, std::size_t len)
		{
			assert(str != nullptr);

			auto err = p_check(str, len);
			if (err != error::ok)
			{
				throw std::runtime_error(g_jsonErrors[std::uint8_t(err)]);
			}
			return { jsonValue::p_parse(str, str + len) };
		}

	public:
		static inline error check(const char * str, std::size_t len = 0) noexcept
		{
			if (len == 0)
			{
				len = std::char_traits<char>::length(str);
			}
			return json::p_check(str, len);
		}
		static inline error check(const std::string & str) noexcept
		{
			return json::p_check(str.c_str(), str.length());
		}

		static inline json parse(const char * str, std::size_t len = 0)
		{
			// If length not given, calc
			if (len == 0)
			{
				len = std::char_traits<char>::length(str);
			}
			// Calling another private recursive function to avoid length-checking
			return json::p_parse(str, len);
		}
		static inline json parse(const std::string & str)
		{
			return json::p_parse(str.c_str(), str.length());
		}
		std::string dump() const
		{
			return this->m_value.dump();
		}

		operator jsonValue & () noexcept
		{
			return this->m_value;
		}
		constexpr operator const jsonValue & () const noexcept
		{
			return this->m_value;
		}
		jsonValue & get() noexcept
		{
			return this->m_value;
		}
		constexpr const jsonValue & get() const noexcept
		{
			return this->m_value;
		}

		constexpr const jsonValue * operator->() const noexcept
		{
			return &this->m_value;
		}
		jsonValue * operator->()
		{
			return &this->m_value;
		}

		jsonKeyValue & operator[](const std::string & key)
		{
			return this->get()[key];
		}
		const jsonKeyValue & operator[](const std::string & key) const
		{
			return this->get()[key];
		}
		jsonKeyValue & at(const std::string & key)
		{
			return this->get().at(key);
		}
		const jsonKeyValue & at(const std::string & key) const
		{
			return this->get().at(key);
		}

		jsonValue & operator[](std::size_t idx)
		{
			return this->get()[idx];
		}
		const jsonValue & operator[](std::size_t idx) const
		{
			return this->get()[idx];
		}
		jsonValue & at(std::size_t idx)
		{
			return this->get()[idx];
		}
		const jsonValue & at(std::size_t idx) const
		{
			return this->get()[idx];
		}
	};

	// Value checking

	inline void json::p_checkValue(const char *& it, const char * end, error & err) noexcept
	{
		assert(it != nullptr);
		assert(end != nullptr);

		bool done = false;
		for (; it != end; ++it)
		{
			switch (*it)
			{
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				break;
			case ']':
			case '}':
				err = error::noValue;
				break;
			case '{':
				json::p_checkObject(it, end, err);
				done = true;
				break;
			case '[':
				json::p_checkArray(it, end, err);
				done = true;
				break;
			case '"':
				++it;
				for (; it != end; ++it)
				{
					if ((*it == '\\') && ((it + 1) != end))
					{
						++it;
						continue;
					}
					else if (*it == '"')
					{
						done = true;
						++it;
						break;
					}
				}
				if (!done)
				{
					err = error::noTerminatingQuote;
				}
				break;
			case 'f':
				if (((end - it) >= 5) && (strncmp(it, "false", 5) == 0))
				{
					it += 5;
					done = true;
				}
				else
				{
					err = error::invalidChar;
				}
				break;
			case 't':
				if (((end - it) >= 4) && (strncmp(it, "true", 4) == 0))
				{
					it += 4;
					done = true;
				}
				else
				{
					err = error::invalidChar;
				}
				break;
			case 'n':
				if (((end - it) >= 4) && (strncmp(it, "null", 4) == 0))
				{
					it += 4;
					done = true;
				}
				else
				{
					err = error::invalidChar;
				}
				break;
			default:
				if ((*it >= '0') && (*it <= '9'))
				{
					done = true;

					bool dot = false;
					bool decAfterDot = false;

					++it;
					for (; it != end; ++it)
					{
						if ((*it >= '0') && (*it <= '9'))
						{
							decAfterDot = dot;
							continue;
						}
						else if (*it == '.')
						{
							if (dot)
							{
								err = error::multipleDecimalPoints;
								break;
							}
							else
							{
								dot = true;
							}
						}
						else
						{
							break;
						}
					}
					if (dot && !decAfterDot)
					{
						err = error::noDigitAfterDecimal;
					}
				}
				else
				{
					err = error::invalidChar;
				}
			}
			if (done || (err != error::ok))
			{
				break;
			}
		}

		// Find for comma, ] or }
		for (; it != end; ++it)
		{
			if ((*it == ' ') || (*it == '\t') || (*it == '\n') || (*it == '\r'))
			{
				continue;
			}
			else if ((*it == ']') || (*it == '}'))
			{
				break;
			}
			else if (*it == ',')
			{
				++it;
				break;
			}
			else
			{
				err = error::invalidTerminator;
				break;
			}
		}
	}
	inline void json::p_checkKeyValue(const char *& it, const char * end, error & err) noexcept
	{
		assert(it != nullptr);
		assert(end != nullptr);

		for (; it != end; ++it)
		{
			if (*it == '"')
			{
				break;
			}
			else if ((*it == ' ') || (*it == '\t') || (*it == '\n') || (*it == '\r'))
			{
				continue;
			}
			else
			{
				err = error::invalidChar;
				return;
			}
		}
		if (it == end)
		{
			err = error::noKey;
			return;
		}

		++it;
		for (; it != end; ++it)
		{
			if ((*it == '\\') && ((it + 1) != end))
			{
				++it;
				continue;
			}
			else if (*it == '"')
			{
				break;
			}
		}
		if (it == end)
		{
			err = error::noTerminatingQuote;
			return;
		}

		++it;
		for (; it != end; ++it)
		{
			if (*it == ':')
			{
				break;
			}
			switch (*it)
			{
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				break;
			default:
				err = error::invalidChar;
				return;
			}
		}

		if (it == end)
		{
			err = error::noValueSeparator;
			return;
		}

		++it;
		json::p_checkValue(it, end, err);
	}
	inline std::size_t json::p_checkValues(const char *& it, const char * end, error & err) noexcept
	{
		assert(it != nullptr);
		assert(end != nullptr);

		bool done = false;
		size_t vals = 0;
		while (it != end)
		{
			switch (*it)
			{
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				++it;
				break;
			case ']':
				if (vals)
				{
					done = true;
					break;
				}
				/* fall through */
			case '}':
				err = error::invalidTerminator;
				break;
			default:
				json::p_checkValue(it, end, err);
				++vals;
			}
			if (done || (err != error::ok))
			{
				break;
			}
		}
		return vals;
	}
	inline void json::p_checkObject(const char *& it, const char * end, error & err) noexcept
	{
		assert(it != nullptr);
		assert(end != nullptr);

		for (; it != end; ++it)
		{
			if (*it == '{')
			{
				break;
			}
			else if ((*it == ' ') || (*it == '\t') || (*it == '\n') || (*it == '\r'))
			{
				continue;
			}
			else
			{
				err = error::invalidChar;
				return;
			}
		}
		if (it == end)
		{
			err = error::noValue;
			return;
		}
		++it;

		bool ended = false;
		while (it != end)
		{
			switch (*it)
			{
			case '"':
				json::p_checkKeyValue(it, end, err);
				break;
			case '}':
				ended = true;
				/* fall through */
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				++it;
				break;
			default:
				err = error::invalidChar;
			}

			if (ended || (err != error::ok))
			{
				break;
			}
		}
		if (!ended && (err == error::ok))
		{
			err = error::noObjectTerminator;
		}
	}
	inline void json::p_checkArray(const char *& it, const char * end, error & err) noexcept
	{
		assert(it != nullptr);
		assert(end != nullptr);

		for (; it != end; ++it)
		{
			if (*it == '[')
			{
				break;
			}
			else if ((*it == ' ') || (*it == '\t') || (*it == '\n') || (*it == '\r'))
			{
				continue;
			}
			else
			{
				err = error::invalidChar;
				return;
			}
		}
		if (it == end)
		{
			err = error::noValue;
			return;
		}
		++it;

		bool ended = false;
		while (it != end)
		{
			switch (*it)
			{
			case ']':
				ended = true;
				/* fall through */
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				++it;
				break;
			default:
				json::p_checkValues(it, end, err);
			}

			if (ended || (err != error::ok))
			{
				break;
			}
		}
		if (!ended && (err == error::ok))
		{
			err = error::noArrayTerminator;
		}
	}

}
