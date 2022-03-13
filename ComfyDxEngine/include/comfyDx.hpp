#pragma once

#ifdef COMFYDXENGINE_EXPORTS
#define COMFYDX_API __declspec(dllexport)
#else
#define COMFYDX_API __declspec(dllimport)
#endif

#include "direct2d.hpp"
#include "directwrite.hpp"

#include <cstdio>

namespace cdx::test
{
	COMFYDX_API int helloWorld() noexcept;
	
}
