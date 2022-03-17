#pragma once

#include <win32Helper.hpp>
#include <argparser.hpp>
#include "argHelper.hpp"

namespace ce
{
	class App
	{
	public:
		HINSTANCE m_instance{ nullptr };
		int m_cmdShow{ SW_SHOW };
		bool m_bHasConsole{ false };

		win32::ArgcArgv m_cmdArgs{};
		argparser::Tokeniser m_argTokeniser{ m_cmdArgs.argc, m_cmdArgs.argv };

		win32::WinConsole m_terminal;

		struct Attributes
		{
			std::uint16_t width{}, height{};

			[[nodiscard]] bool init(argparser::Tokeniser & tok) noexcept;
		} m_attributes{};

		bool m_bCanRun{ true };

		App() = delete;
		App(HINSTANCE hInst, int nCmdShow);
		~App() noexcept;


		[[nodiscard]] bool initTerminal() noexcept;
		[[nodiscard]] bool initAttributes() noexcept;
		int run();
	};
}
