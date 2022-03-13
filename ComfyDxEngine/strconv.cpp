#include "pch.hpp"
#include "strconv.hpp"

COMFYDX_API std::string cet::conv(std::wstring_view str)
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
COMFYDX_API std::wstring cet::conv(std::string_view str)
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
