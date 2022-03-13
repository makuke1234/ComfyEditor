#pragma once

#include <argparser.hpp>
#include <limits>

#include "concepts.hpp"
#include "strconv.hpp"

namespace cet
{
	template<concepts::numeric N>
	[[nodiscard]] std::string getDefArgStr(const argparser::Token & token, N defArg)
	{
		return token.getDef(std::to_string(defArg));
	}

	template<std::integral I>
	[[nodiscard]] I getDefArg(const argparser::Token & token, I defArg) noexcept
	{
		try
		{
			return cet::stoi<I>(cet::getDefArgStr(token, defArg));
		}
		catch (const std::exception &)
		{
			return std::numeric_limits<I>::max();
		}
	}
	template<std::floating_point F>
	[[nodiscard]] F getDefArg(const argparser::Token & token, F defArg) noexcept
	{
		try
		{
			return cet::stof<F>(cet::getDefArgStr(token, defArg));
		}
		catch (const std::exception &)
		{
			return std::numeric_limits<F>::max();
		}
	}

}
