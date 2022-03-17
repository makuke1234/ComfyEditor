#include "pch.hpp"
#include "app.hpp"

ce::App::App(HINSTANCE hInst, int nCmdShow)
	: m_instance{ hInst }, m_cmdShow{ nCmdShow }
{
	this->m_bHasConsole = ce::getDefArg<bool>(this->m_argTokeniser.tokenise(argparser::regex::dashTemplate("console="), 1), false);
	if (this->m_bHasConsole)
	{
		this->m_bCanRun = this->initTerminal();
	}

	this->m_bCanRun = this->m_bCanRun && this->initAttributes();
}
ce::App::~App() noexcept
{
	if (this->m_bHasConsole)
	{
		std::cout << "Press any key to exit..." << std::endl;
		std::cin.get();
	}
}

[[nodiscard]] bool ce::App::initTerminal() noexcept
{
	return this->m_terminal.attach();
}
[[nodiscard]] bool ce::App::initAttributes() noexcept
{
	return this->m_attributes.init(this->m_argTokeniser);
}
[[nodiscard]] bool ce::App::Attributes::init(argparser::Tokeniser & tok) noexcept
{
	this->width = ce::getDefArg<std::uint16_t>(tok.tokenise(argparser::regex::dashTemplate("width=", "Width="), 1), 512);
	
	return true;
}

int ce::App::run()
{
	if (!this->m_bCanRun)
	{
		return 1;
	}

	std::cout << "Width attribute: " << this->m_attributes.width << std::endl;

	return 0;
}
