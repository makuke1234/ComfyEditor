#pragma once

#include <argparser.hpp>
#include <limits>

#include "concepts.hpp"
#include "strconv.hpp"

namespace ce
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
			return ce::stoi<I>(ce::getDefArgStr(token, defArg));
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
			return ce::stof<F>(ce::getDefArgStr(token, defArg));
		}
		catch (const std::exception &)
		{
			return std::numeric_limits<F>::max();
		}
	}

}
