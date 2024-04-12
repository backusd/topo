#pragma once
#include "topo/Core.h"


namespace topo
{
TOPO_API ND std::wstring s2ws(const std::string& str) noexcept;
TOPO_API ND std::string ws2s(const std::wstring& wstr) noexcept;
TOPO_API ND bool ends_with(std::string_view str, std::string_view suffix) noexcept;

}