#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <d2d1.h>
#include <dwrite.h>

#include "comfyDx.hpp"

namespace cdx::win32
{
	class NonDX
	{
	public:
		using HubProc = WNDPROC;
		using CustomProc = LRESULT(*)(void * data, HWND hwnd, UINT umsg, WPARAM wp, LPARAM lp);
	
	private:
		static LRESULT CALLBACK winProc_hub(HWND hwnd, UINT umsg, WPARAM wp, LPARAM lp);
		static LRESULT winProc(void * data, HWND hwnd, UINT umsg, WPARAM wp, LPARAM lp);

	public:
		HubProc m_procHub = &winProc_hub;
		CustomProc m_procMod = &winProc;
		void * m_procModData = this;

		NonDX() = default;
		~NonDX() noexcept;
	};
}
