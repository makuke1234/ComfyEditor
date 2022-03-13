/*
 * argument vector parser
 * @author Maku Maku
 * @details There's basically nothing special to it apart from using regex.
 *
 */

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <limits>
#include <optional>
#include <regex>
#include <utility>

#include <cstdint>

#if __cplusplus > 201703L
#define CPP20
#endif


namespace argparser
{
	namespace regex
	{
		namespace private_
		{
#ifdef CPP20

			template<size_t N>
			struct StringLiteral
			{
				constexpr StringLiteral(const char(&str)[N]) noexcept
				{
					std::copy_n(str, N, value);
				}

				char value[N];
			};

#endif

			[[nodiscard]] inline std::string dashTemplateVecAppend(std::string_view str1)
			{
				std::string buildStr{ "(?:" };
				buildStr.append(str1.cbegin(), str1.cend());
				buildStr.append(")");

				return buildStr;
			}

			template<class ... Args>
			[[nodiscard]] std::string dashTemplateVecAppend(std::string_view str1, Args && ... args)
			{
				std::string buildStr{ "(?:" };
				buildStr.append(str1.cbegin(), str1.cend());
				buildStr.append(")|");

				buildStr.append(private_::dashTemplateVecAppend(std::forward<Args>(args)...));

				return buildStr;
			}
		}

#ifdef CPP20
		template<private_::StringLiteral regex = R"reg((.+))reg">
#endif
		[[nodiscard]] inline std::string dashTemplate(std::string_view variableStr)
		{
			std::string buildStr{ R"reg(^(?:--?|/))reg" };
			buildStr.append(variableStr.cbegin(), variableStr.cend());
#ifdef CPP20
			buildStr.append(regex);
#else
			buildStr.append(R"reg((.+))reg");
#endif
			return buildStr;
		}

#ifdef CPP20
		template<private_::StringLiteral regex = R"reg((.+))reg", class ... Args>
#else
		template<class ... Args>
#endif
		[[nodiscard]] std::string dashTemplate(Args && ... args)
		{
			std::string buildStr{ R"reg(^(?:--?|/)(?:)reg" };

			buildStr.append(private_::dashTemplateVecAppend(std::forward<Args>(args)...));

			buildStr += ')';
#ifdef CPP20
			buildStr.append(regex.value);
#else
			buildStr.append(R"reg((.+))reg");
#endif
			return buildStr;
		}
	}

	class Tokeniser;

	class Token
	{
	private:
		friend class argparser::Tokeniser;

		std::optional<std::string> m_token;

		Token() noexcept = default;
		Token(const std::string & token)
			: m_token(token)
		{}
		explicit constexpr Token(std::string && token) noexcept
			: m_token(std::move(token))
		{}

	public:
		[[nodiscard]] operator std::string() const noexcept
		{
			return this->m_token.value_or("");
		}
		[[nodiscard]] std::string get() const noexcept
		{
			return this->m_token.value_or("");
		}
		[[nodiscard]] std::string getDef(const std::string & def) const
		{
			return this->m_token.value_or(def);
		}

		[[nodiscard]] explicit constexpr operator bool() const noexcept
		{
			return this->m_token.has_value();
		}
		[[nodiscard]] constexpr bool operator==(bool rhs) const noexcept
		{
			return this->m_token.has_value() == rhs;
		}
		[[nodiscard]] constexpr bool operator!=(bool rhs) const noexcept
		{
			return !this->operator==(rhs);
		}
	};

	class Tokeniser
	{
	public:
		using MatchType = typename std::match_results<std::string_view::const_iterator>;
		static constexpr std::size_t any{ std::numeric_limits<std::size_t>::max() };

	private:
		std::vector<std::string_view> m_args;


	public:
		Tokeniser() noexcept = default;
		Tokeniser(int argc, const char * const * argv)
			: m_args{ argv, argv + argc }
		{}
		Tokeniser(std::pair<int, char **> argPair)
			: m_args{ argPair.second, argPair.second + argPair.first }
		{}
		Tokeniser(const Tokeniser &) = delete;
		Tokeniser(Tokeniser && other) noexcept = default;
		Tokeniser & operator=(const Tokeniser &) = delete;
		Tokeniser & operator=(Tokeniser && other) noexcept = default;
		~Tokeniser() noexcept = default;

		Token tokenise(
			std::string_view pattern,
			std::size_t fromArg = Tokeniser::any,
			std::size_t toArg = Tokeniser::any
		) noexcept
		{
			if (this->m_args.empty())
				return {};

			std::regex patternExp{ pattern.begin(), pattern.end() };
			auto scanToken = [patternExp](std::string_view input) noexcept -> Token
			{
				// Do regex checking
				MatchType sm;
				if (auto ret{ std::regex_match(input.cbegin(), input.cend(), sm, patternExp) }; ret == true && sm.size() > 1)
					return sm.str(sm.size() - 1);
				else
					return {};
			};

			fromArg = (fromArg == Tokeniser::any) ? 0 : std::min(fromArg, this->m_args.size() - 1);
			toArg = std::min(toArg, this->m_args.size());

			for (auto it = this->m_args.cbegin() + fromArg, end = this->m_args.cbegin() + toArg; it != end; ++it)
			{
				// Scan the pattern
				if (auto ret{ scanToken(*it) }; ret == true)
					return ret;
			}

			return {};
		}
	};
}
