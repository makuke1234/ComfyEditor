#include "pch.hpp"
#include "app.hpp"

int WINAPI wWinMain(HINSTANCE hInst, [[maybe_unused]] HINSTANCE hPrevInst, [[maybe_unused]] LPWSTR lpCmdArgs, int nCmdShow)
{
	return ce::App{ hInst, nCmdShow }.run();
}
