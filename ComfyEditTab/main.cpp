#include "pch.hpp"

#include "argHelper.hpp"

#include <iostream>

int main(int argc, char ** argv)
{
	argparser::Tokeniser tokeniser(argc, argv);
	
	auto width = cet::getDefArg<std::uint16_t>(tokeniser.tokenise(argparser::regex::dashTemplate("width=", "Width="), 1), 512);
	std::cerr << "Width attribute: " << width;
	
	return 0;
}
