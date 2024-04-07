#pragma once
#include "topo/Core.h"

namespace topo
{
#ifdef TOPO_PLATFORM_WINDOWS
ND std::string TranslateErrorCode(HRESULT hr) noexcept;
#else
#error Only Supporting Windows!
#endif
}