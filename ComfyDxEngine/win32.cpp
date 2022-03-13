#include "pch.hpp"
#include "win32.hpp"

namespace cdx::win32
{
	LRESULT CALLBACK NonDX::winProc_hub(HWND hwnd, UINT umsg, WPARAM wp, LPARAM lp)
	{

		return ::DefWindowProcW(hwnd, umsg, wp, lp);
	}
	LRESULT NonDX::winProc(void * data, HWND hwnd, UINT umsg, WPARAM wp, LPARAM lp)
	{

		return ::DefWindowProcW(hwnd, umsg, wp, lp);
	}

	NonDX::~NonDX() noexcept
	{

	}
}