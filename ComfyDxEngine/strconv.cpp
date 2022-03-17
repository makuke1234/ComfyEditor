#include "pch.hpp"
#include "strconv.hpp"

COMFYDX_API std::string ce::conv(std::wstring_view str)
{
	int realSize = ::WideCharToMultiByte(
		CP_UTF8,
		0,
		str.data(),
		str.size() + 1,
		nullptr,
		0,
		nullptr,
		nullptr
	);

	std::string out;
	out.resize(realSize - 1);

	::WideCharToMultiByte(
		CP_UTF8,
		0,
		str.data(),
		str.size() + 1,
		out.data(),
		realSize,
		nullptr,
		nullptr
	);
	return out;
}
COMFYDX_API std::wstring ce::conv(std::string_view str)
{
	int realSize = ::MultiByteToWideChar(
		CP_UTF8,
		MB_PRECOMPOSED,
		str.data(),
		str.size() + 1,
		nullptr,
		0
	);

	std::wstring out;
	out.resize(realSize - 1);

	::MultiByteToWideChar(
		CP_UTF8,
		MB_PRECOMPOSED,
		str.data(),
		str.size() + 1,
		out.data(),
		realSize
	);

	return out;
}

template<> COMFYDX_API float ce::stof<float>(std::string_view str, std::size_t * pos)
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
template<> COMFYDX_API double ce::stof<double>(std::string_view str, std::size_t * pos)
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
template<> COMFYDX_API long double ce::stof<long double>(std::string_view str, std::size_t * pos)
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