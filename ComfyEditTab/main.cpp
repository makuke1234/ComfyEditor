#include "pch.hpp"

#include "argHelper.hpp"

#include <iostream>

int WINAPI wWinMain(HINSTANCE hInst, [[maybe_unused]] HINSTANCE hPrevInst, [[maybe_unused]] LPWSTR lpCmdArgs, int nCmdShow)
{
	win32::ArgcArgv args;

	argparser::Tokeniser tokeniser(args.argc, args.argv);
	
	auto console = cet::getDefArg<bool>(tokeniser.tokenise(argparser::regex::dashTemplate("console="), 1), false);
	win32::WinConsole winConsole;

	if (console)
	{
		// Attach to parent process console
		if (!winConsole.attach()) [[unlikely]]
		{
			::MessageBoxW(nullptr, L"Error creating console!", APP_NAME, MB_ICONERROR | MB_OK);
			return 1;
		}
	}

	auto width = cet::getDefArg<std::uint16_t>(tokeniser.tokenise(argparser::regex::dashTemplate("width=", "Width="), 1), 512);
	std::cout << "Width attribute: " << width << std::endl;

	
	if (console)
	{
		std::cout << "Press any key to exit..." << std::endl;
		std::cin.get();
	}

	return 0;
}
