#pragma once

#include <string_view>
#include <string>
#include <concepts>
#include <cstdlib>
#include <stdexcept>

#include "concepts.hpp"

namespace cet
{
	template<std::floating_point F>
	F stof(std::string_view str, std::size_t * pos = nullptr);

	template<> float stof<float>(std::string_view str, std::size_t * pos)
	{
		char * ptr;
		auto val = std::strtof(str.data(), &ptr);
		if (str.data() == ptr)
		{
			throw std::invalid_argument("stof<float>");
		}
		else if (errno == ERANGE)
		{
			throw std::out_of_range("stof<float>");
		}
		else if (pos != nullptr)
		{
			*pos = std::size_t(ptr - str.data());
		}
		return val;
	}
	template<> double stof<double>(std::string_view str, std::size_t * pos)
	{
		char * ptr;
		auto val = std::strtod(str.data(), &ptr);
		if (str.data() == ptr)
		{
			throw std::invalid_argument("stof<double>");
		}
		else if (errno == ERANGE)
		{
			throw std::out_of_range("stof<double>");
		}
		else if (pos != nullptr)
		{
			*pos = std::size_t(ptr - str.data());
		}
		return val;
	}
	template<> long double stof<long double>(std::string_view str, std::size_t * pos)
	{
		char * ptr;
		auto val = std::strtold(str.data(), &ptr);
		if (str.data() == ptr)
		{
			throw std::invalid_argument("stof<long double>");
		}
		else if (errno == ERANGE)
		{
			throw std::out_of_range("stof<long double>");
		}
		if (pos != nullptr)
		{
			*pos = std::size_t(ptr - str.data());
		}
		return val;
	}

	template<concepts::signed_integral_until32 SI>
	SI stoi(std::string_view str, std::size_t * pos = nullptr, int base = 10)
	{
		char * ptr;
		auto val = std::strtol(str.data(), &ptr, base);
		if (str.data() == ptr)
		{
			throw std::invalid_argument("stoi");
		}
		else if ((errno == ERANGE) || (val < std::numeric_limits<SI>::min()) || (val > std::numeric_limits<SI>::max()))
		{
			throw std::out_of_range("stoi");
		}
		else if (pos != nullptr)
		{
			*pos = std::size_t(ptr - str.data());
		}
		return static_cast<SI>(val);
	}
	template<concepts::int64_t I64>
	I64 stoi(std::string_view str, std::size_t * pos = nullptr, int base = 10)
	{
		char * ptr;
		I64 val = std::strtoll(str.data(), &ptr, base);
		if (pos != nullptr)
		{
			*pos = std::size_t(ptr - str.data());
		}
		else if (errno == ERANGE)
		{
			throw std::out_of_range("stoi");
		}
		else if (pos != nullptr)
		{
			*pos = std::size_t(ptr - str.data());
		}
		return val;
	}

	template<concepts::unsigned_integral_until32 UI>
	UI stoi(std::string_view str, std::size_t * pos = nullptr, int base = 10)
	{
		char * ptr;
		auto val = std::strtoul(str.data(), &ptr, base);
		if (pos != nullptr)
		{
			*pos = std::size_t(ptr - str.data());
		}
		else if ((errno == ERANGE) || (val > static_cast<unsigned long>(std::numeric_limits<UI>::max())))
		{
			throw std::out_of_range("stoi");
		}
		if (pos != nullptr)
		{
			*pos = std::size_t(ptr - str.data());
		}
		return static_cast<UI>(val);
	}
	template<concepts::uint64_t U64>
	U64 stoi(std::string_view str, std::size_t * pos = nullptr, int base = 10)
	{
		char * ptr;
		U64 val = std::strtoull(str.data(), &ptr, base);
		if (pos != nullptr)
		{
			*pos = std::size_t(ptr - str.data());
		}
		else if (errno == ERANGE)
		{
			throw std::out_of_range("stoi");
		}
		if (pos != nullptr)
		{
			*pos = std::size_t(ptr - str.data());
		}
		return val;
	}

	std::string conv(std::wstring_view str);
	std::wstring conv(std::string_view str);
}
